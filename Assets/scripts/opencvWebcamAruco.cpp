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
 * Looks for Aruco markers live through webcam and draws axis lines onto each found
 */
int webcamArucoDetection(const Mat& cameraMatrix, const Mat& distanceCoefficients, float arucoSquareDimensions) {
	Mat frame;
	vector<int> markerIds;						// to store the ids of any aruco markers found
	vector<vector<Point2f>> markerCorners;		// to store points (x,y) of the 4 corners of each aruco marker found
	vector<Vec3d> rotationVectors;				// to store the rotation vector of each aruco marker found to go to 3D
	vector<Vec3d> translationVectors;			// to store the translation vector of each aruco marker found to go to 3D
	aruco::DetectorParameters parameters;

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

		// Estimates the 3D position of each Aruco marker based on camera calibration
		aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDimension, cameraMatrix, distanceCoefficients, rotationVectors, translationVectors);

		for (int i = 0; i < markerIds.size(); i++) {
			aruco::drawAxis(frame, cameraMatrix, distanceCoefficients, rotationVectors[i], translationVectors[i], 0.1f);
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

