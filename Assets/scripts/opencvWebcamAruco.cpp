#include "opencv2\core.hpp"
#include "opencv2\imgcodecs.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\aruco.hpp"
#include "opencv2\calib3d.hpp"

#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

const float calibrationSquareDimension = 0.026f;	//measured chessboard square length in meters
const float arucoSquareDimension = 0.04f;			//measured aruco marker square length in meters
const Size chessboardDimensions = Size(6, 9);		//number of corners to the calibration chessboard

/*
 * Create 50 4x4 aruco markers and save them in current directory
 */
void createArucoMarkers() {
	Mat outputMarker;
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

	for (int i = 0; i < 50; i++) {
		aruco::drawMarker(markerDictionary,i,500,outputMarker,1);
		ostringstream convert;
		string imageName = "4x4Marker_";
		convert << imageName << i << ".jpg";
		imwrite(convert.str(),outputMarker);
	}
}

/*
 * Store actual points (x,y,0) of the 6x9 chessboard corners for camera calibration based on measurements
 */
void createKnownBoardPosition(Size boardSize, float squareEdgeLength, vector<Point3f>& corners) {
	for (int i = 0; i < boardSize.height; i++) {
		for (int j = 0; j < boardSize.width; j++) {
			corners.push_back(Point3f(j*squareEdgeLength, i*squareEdgeLength, 0.0f));
		}
	}
}

/*
 * Calls openCV's findChessBoardCorners() to find chessboard corners (x,y) from calibration images and stores them
 */
void getChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& allFoundCorners) {
	for (vector<Mat>::iterator iter = images.begin(); iter != images.end(); iter++) {
		vector<Point2f> pointBuf;
		bool found = findChessboardCorners(*iter, Size(9, 6), pointBuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);

		if (found) {
			allFoundCorners.push_back(pointBuf);
		}
	}
}

/*
 * Calls openCv's calibrateCamera() to get cameraMatrix and distanceCoefficients needed to get location of aruco markers
 */
void cameraCalibration(vector<Mat> calibrationImages, Size boardSize, float squareEdgeLength, Mat& cameraMatrix, Mat& distanceCoefficients) {
	vector<vector<Point2f>> chessboardImageSpacePoints;
	getChessboardCorners(calibrationImages, chessboardImageSpacePoints);

	vector<vector<Point3f>> worldSpaceCornerPoints(1);

	createKnownBoardPosition(boardSize, squareEdgeLength, worldSpaceCornerPoints[0]);
	worldSpaceCornerPoints.resize(chessboardImageSpacePoints.size(), worldSpaceCornerPoints[0]);

	vector<Mat> rVectors, tVectors; //radial vectors, tangential vectors
	distanceCoefficients = Mat::zeros(8, 1, CV_64F);

	calibrateCamera(worldSpaceCornerPoints, chessboardImageSpacePoints, boardSize, cameraMatrix, distanceCoefficients, rVectors, tVectors);
}

/*
 * Saves cameraMatrix and distanceCoefficients data to a file if you already went through camera calibration process
 */
bool saveCameraCalibration(string name, Mat cameraMatrix, Mat distanceCoefficients) {
	ofstream outStream(name);
	if (outStream) {
		uint16_t rows = cameraMatrix.rows;
		uint16_t columns = cameraMatrix.cols;

		outStream << rows << endl;
		outStream << columns << endl;

		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				double value = cameraMatrix.at<double>(r, c);
				outStream << value << endl;
			}
		}

		rows = distanceCoefficients.rows;
		columns = distanceCoefficients.cols;

		outStream << rows << endl;
		outStream << columns << endl;

		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				double value = distanceCoefficients.at<double>(r, c);
				outStream << value << endl;
			}
		}

		outStream.close();
		return true;
	}
	return false;
}

/*
 * Goes through the camera calibration live through webcam
 * When chessboard corners detected hit space to store frame, and when at least 16 frames are stored hit enter to calibrate
 * Hit esc when you are finished
 */
void cameraCalibrationProcess(Mat& cameraMatrix, Mat& distanceCoefficients) {
	Mat frame;
	Mat drawToFrame;

	vector<Mat> savedImages;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;

	VideoCapture vid(0); // 0 - webcam

	if (!vid.isOpened()) {
		return;
	}

	int framesPerSecond = 20;
	namedWindow("Webcam", CV_WINDOW_AUTOSIZE);

	while (true) {
		if (!vid.read(frame)) {
			return;
		}

		vector<Vec2f> foundPoints;
		bool found = false;

		found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		frame.copyTo(drawToFrame);
		drawChessboardCorners(drawToFrame, chessboardDimensions, foundPoints, found);

		if (found) {
			imshow("Webcam", drawToFrame);
		}
		else {
			imshow("Webcam", frame);
		}
		char character = waitKey(1000 / framesPerSecond);

		switch (character) {
			//space key => saving image
			case ' ':
				if (found) {
					Mat temp;
					frame.copyTo(temp);
					savedImages.push_back(temp);
				}
				break;
			//enter key => start calibration
			case 13:
				if (savedImages.size() > 15) {
					cameraCalibration(savedImages, chessboardDimensions, calibrationSquareDimension, cameraMatrix, distanceCoefficients);
					saveCameraCalibration("CameraCalibration", cameraMatrix, distanceCoefficients);
				}
				break;
			//esc key => exit
			case 27:
				return;
				break;
			}
	}
}

/*
 * Loads the calibration data from a file into cameraMatrix and distanceCoefficients
 */
bool loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients) {
	ifstream inStream(name);
	if (inStream) {
		uint16_t rows;
		uint16_t columns;

		inStream >> rows;
		inStream >> columns;

		cameraMatrix = Mat(Size(columns, rows), CV_64F);
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				double read = 0.0f;
				inStream >> read;
				cameraMatrix.at<double>(r, c) = read;
				//cout << cameraMatrix.at<double>(r,c) << "\n";
			}
		}

		inStream >> rows;
		inStream >> columns;

		distanceCoefficients = Mat::zeros(rows, columns, CV_64F);
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				double read = 0.0f;
				inStream >> read;
				distanceCoefficients.at<double>(r, c) = read;
				//cout << distanceCoefficients.at<double>(r, c) << "\n";
			}
		}

		inStream.close();
		return true;
	}
	return false;
}

/*
 * Sets the Aruco Board with our measured dimensions in the layout:
 * 0   1
 * 2   3
 */
void setArucoBoard(Ptr<aruco::Board>& board) {
	int markersX = 2;							//number of markers horizontally
	int markersY = 2;							//number of markers vertically
	float markerLength = arucoSquareDimension;	//length of marker in metres
	float markerSepX = 0.148f;					//length of horizontal space between markers in metres (measured)
	float markerSepY = 0.085f;					//length of vertical space between markers in metres (measured)

	//get corners of each marker (4 markers => 4*4 corners)
	vector<vector<Point3f>> boardPoints;
	float maxY = (float)markersY * markerLength + (markersY - 1) * markerSepY;
	for (int y = 0; y < markersY; y++) {
		for (int x = 0; x < markersX; x++) {
			vector<Point3f> corners;
			corners.resize(4);
			corners[0] = Point3f(x * (markerLength + markerSepX), maxY - y * (markerLength + markerSepY), 0);
			corners[1] = corners[0] + Point3f(markerLength, 0, 0);
			corners[2] = corners[0] + Point3f(markerLength, -markerLength, 0);
			corners[3] = corners[0] + Point3f(0, -markerLength, 0);
			boardPoints.push_back(corners);
		}
	}

	//which markers?
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

	//which ids?
	vector<int> ids;
	for (int i = 0; i < 4; i++) 
		ids.push_back(i);

	//create board with this information
	board = aruco::Board::create(boardPoints, markerDictionary, ids);
}

/*
 * Looks for Aruco markers live through webcam and draws axis lines onto each found
 */
int webcamArucoDetection(const Mat& cameraMatrix, const Mat& distanceCoefficients, float arucoSquareDimensions) {
	Mat frame;
	vector<int> markerIds;						// to store the ids of any aruco markers found
	vector<vector<Point2f>> markerCorners;		// to store points (x,y) of the 4 corners of each aruco marker found
	Ptr<aruco::Board> arucoBoard;				// to store the layout of our aruco board

	setArucoBoard(arucoBoard);

	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

	VideoCapture vid(0); // 0 - webcam

	if (!vid.isOpened()) {
		return -1;
	}

	namedWindow("Webcam", CV_WINDOW_AUTOSIZE);

	while (true) {
		if (!vid.read(frame)) {
			break;
		}

		// 2D Aruco marker detection of the current frame
		aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIds);

		// Estimates the 3D position of Aruco board (i.e. the plane) based on camera calibration
		if (markerIds.size() > 0) {
			aruco::drawDetectedMarkers(frame, markerCorners, markerIds);

			Vec3d rvec, tvec; //rotation and translation vectors of board
			int valid = aruco::estimatePoseBoard(markerCorners, markerIds, arucoBoard, cameraMatrix, distanceCoefficients, rvec, tvec);

			if (valid > 0) {
				aruco::drawAxis(frame, cameraMatrix, distanceCoefficients, rvec, tvec, 0.1f);
				Mat rotationMatrix;
				Rodrigues(rvec, rotationMatrix); //converts rotation vector to rotation matrix
			}

			/*
			DATA TO PASS TO UNITY
			tvec - position of bottom left corner
			rotationMatrix - needs to be converted to Quaternion in unity?
			*/
		}

		imshow("Webcam", frame);
		if (waitKey(30) >= 0) {
			break;
		}
	}
	return 1;
}

int main(int argv, char** argc) {
	//createArucoMarkers();

	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	Mat distanceCoefficients;

	//cameraCalibrationProcess(cameraMatrix, distanceCoefficients);
	loadCameraCalibration("CameraCalibration", cameraMatrix, distanceCoefficients);

	webcamArucoDetection(cameraMatrix, distanceCoefficients, arucoSquareDimension);
	return 0;
}

