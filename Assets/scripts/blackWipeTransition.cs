using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class blackWipeTransition : MonoBehaviour {

	public int state;
	public int maxtime;
	public int timer;
	public int direction;
	public string destination;
	public GameObject wipe;
	public float factor; 

	public bool isCanvas;

	public float zLevel;

	// Use this for initialization
	void Start () {
//		DontDestroyOnLoad (this.gameObject);
		//this.GetComponent<CanvasRenderer>().renderer.sortingLayerName = 5;
		maxtime = timer;
		//rolloid = deactivate;
		SceneManager.sceneLoaded += reverse; 
	}
	
	// Update is called once per frame
	void Update () {
		if (isCanvas) { // canvas mode 
			if (state == 1) {
				factor = ((float)maxtime - (float)timer) / (float)maxtime;
				switch (direction) {
				case 0: 
					factor = factor * (Screen.width) ;
					this.GetComponent<RectTransform>().localScale = new Vector3 (factor,(Screen.height),1);
					this.GetComponent<RectTransform>().position = new Vector3 (((factor) - Screen.width / 2f), (Screen.height/2f), zLevel);
					break;
				case 1:
					factor = factor * (Screen.height) ;
					this.GetComponent<RectTransform>().localScale = new Vector3 (Screen.width,factor,1);
					this.GetComponent<RectTransform>().position = new Vector3 ( (Screen.width/2f), ((factor) - Screen.height / 2f), zLevel);
					break;
				case 2:
					factor = factor * (Screen.width) ;
					this.GetComponent<RectTransform>().localScale = new Vector3 (factor,(Screen.height),1);
					this.GetComponent<RectTransform>().position = new Vector3 (((-factor/2f) + Screen.width), (Screen.height/2f), zLevel);
					break;
				case 3:
					factor = factor * (Screen.height) ;
					this.GetComponent<RectTransform>().localScale = new Vector3 ((Screen.width), factor, 1);
					this.GetComponent<RectTransform>().position = new Vector3 ((Screen.width/2f), ((-factor/2f) + Screen.height), zLevel);
					break;
				}
				if (timer == 0) {
					SceneManager.LoadScene (destination);
				} else {
					timer--;
				}
			} else if (state == 2) {
				factor = (float)timer / (float)maxtime;
				switch (direction) {
				case 0: 
					factor = factor * (Screen.width) ;
					this.GetComponent<RectTransform>().localScale = new Vector3 (factor,(Screen.height),1);
					this.GetComponent<RectTransform>().position = new Vector3 (((-factor/2f) + Screen.width), (Screen.height/2f), zLevel);
					break;
				case 1:
					factor = factor * (Screen.height) ;
					this.GetComponent<RectTransform>().localScale = new Vector3 ((Screen.width), factor, 1);
					this.GetComponent<RectTransform>().position = new Vector3 ((Screen.width/2f), ((-factor/2f) + Screen.height), zLevel);
					break;
				case 2:
					factor = factor * (Screen.width) ;
					this.GetComponent<RectTransform>().localScale = new Vector3 (factor,(Screen.height),1);
					this.GetComponent<RectTransform>().position = new Vector3 (((factor) - Screen.width / 2f), (Screen.height/2f), zLevel);
					break;
				case 3:
					factor = factor * (Screen.height) ;
					this.GetComponent<RectTransform>().localScale = new Vector3 (Screen.width,factor,1);
					this.GetComponent<RectTransform>().position = new Vector3 ( (Screen.width/2f), ((factor) - Screen.height / 2f), zLevel);
					break;
				}
				if (timer == 0) {
					state = 0;
					timer = maxtime;
				} else {
					timer--;
				}
			} //else if (state == 0) {
//				wipe.transform.position = new Vector3 (-1000, -1000, 0);
//				wipe.trans
//			}
		} else { // Gameobject Mode
			if (state == 1) {
				factor = ((float)maxtime - (float)timer) / (float)maxtime;
				switch (direction) {
				case 0: 
					wipe.transform.position = wipe.transform.position + new Vector3 (((Screen.width / 32f) / (float)maxtime), 0, 0);
					break;
				case 1:
					wipe.transform.position = wipe.transform.position + new Vector3 (0, (-(Screen.width / 32f) / (float)maxtime), 0);
					break;
				case 2:
					wipe.transform.position = wipe.transform.position + new Vector3 ((-(Screen.width / 32f) / (float)maxtime), 0, 0);
					break;
				case 3:
					wipe.transform.position = wipe.transform.position + new Vector3 (0, ((Screen.width / 32f) / (float)maxtime), 0);
					break;
				}
				if (timer == 0) {
					SceneManager.LoadScene (destination);
				} else {
					timer--;
				}
			} else if (state == 2) {
				factor = ((float)maxtime - (float)timer) / (float)maxtime;
				switch (direction) {
				case 0: 
					wipe.transform.position = wipe.transform.position + new Vector3 (((Screen.width / 32f) / (float)maxtime), 0, 0);
					break;
				case 1:
					wipe.transform.position = wipe.transform.position + new Vector3 (0, (-(Screen.width / 32f) / (float)maxtime), 0);
					break;
				case 2:
					wipe.transform.position = wipe.transform.position + new Vector3 ((-(Screen.width / 32f) / (float)maxtime), 0, 0);
					break;
				case 3:
					wipe.transform.position = wipe.transform.position + new Vector3 (0, ((Screen.width / 32f) / (float)maxtime), 0);
					break;
				}
				if (timer == 0) {
					state = 0;
					timer = maxtime;
				} else {
					timer--;
				}
			} else if (state == 0) {
				wipe.transform.position = new Vector3 (100, 100, 0);
			}
		}
	}

	private void reverse (Scene scene, LoadSceneMode mode) {
		if (isCanvas) {
			state = 2;
			timer = maxtime;
			//		wipe.transform.position = new Vector3 (1000, 1000, 5);
			switch (direction) {
			case 0: 
				this.GetComponent<RectTransform>().localScale = new Vector3 (0,(Screen.height),1);
				this.GetComponent<RectTransform>().position = new Vector3 ((Screen.width / 2), 0, zLevel);
				break;
			case 1:
				this.GetComponent<RectTransform>().localScale = new Vector3 ((Screen.width),0,1);
				this.GetComponent<RectTransform>().position = new Vector3 (0, (-Screen.height / 2), zLevel);
				break;
			case 2:
				this.GetComponent<RectTransform>().localScale = new Vector3 (0,(Screen.height),1);
				this.GetComponent<RectTransform>().position = new Vector3 ((-Screen.width / 2), 0, zLevel);
				break;
			case 3:
				this.GetComponent<RectTransform>().localScale = new Vector3 ((Screen.width),0,1);
				this.GetComponent<RectTransform>().position = new Vector3 (0, (Screen.height / 2), zLevel);
				break;
			}
		} else {
			state = 2;
			timer = maxtime;
//		wipe.transform.position = new Vector3 (1000, 1000, 5);
			switch (direction) {
			case 0: 
				wipe.transform.position = Camera.main.transform.position + new Vector3 ((-(Screen.width / 64) + 32), 0, 6);
				break;
			case 1:
				wipe.transform.position = Camera.main.transform.position + new Vector3 (0, ((Screen.height / 64) - 32), 6);
				break;
			case 2:
				wipe.transform.position = Camera.main.transform.position + new Vector3 (((Screen.width / 64) - 32), 0, 6);
				break;
			case 3:
				wipe.transform.position = Camera.main.transform.position + new Vector3 (0, (-(Screen.height / 64) + 32), 6);
				break;
			}
		}
	}

	public void LoadScene (int dir, string dest) {
		if (isCanvas) {
			if (state == 0) {
				state = 1;
				direction = dir;
				destination = dest;  
				switch (direction) {
				case 0: 
					this.GetComponent<RectTransform>().localScale = new Vector3 (0,(Screen.height),1);
					this.GetComponent<RectTransform>().position = new Vector3 ((-Screen.width / 2), (Screen.height/2f), zLevel);
					break;
				case 1:
					this.GetComponent<RectTransform>().localScale = new Vector3 ((Screen.width),0,1);
					this.GetComponent<RectTransform>().position = new Vector3 ((Screen.width / 2), (Screen.height / 2), zLevel);
					break;
				case 2:
					this.GetComponent<RectTransform>().localScale = new Vector3 (0,(Screen.height),1);
					this.GetComponent<RectTransform>().position = new Vector3 ((Screen.width / 2), (Screen.height/2f), zLevel);
					break;
				case 3:
					this.GetComponent<RectTransform>().localScale = new Vector3 ((Screen.width),0,1);
					this.GetComponent<RectTransform>().position = new Vector3 ((Screen.width / 2), (-Screen.height / 2), zLevel);
					break;
				}
			}
		} else {
			if (state == 0) {
				state = 1;
				direction = dir;
				destination = dest;  
				switch (direction) {
				case 0: 
					wipe.transform.position = Camera.main.transform.position + new Vector3 (((-Screen.width / 64) - 32), 0, 6);
					break;
				case 1:
					wipe.transform.position = Camera.main.transform.position + new Vector3 (0, ((Screen.height / 64) + 32), 6);
					break;
				case 2:
					wipe.transform.position = Camera.main.transform.position + new Vector3 (((Screen.width / 64) + 32), 0, 6);
					break;
				case 3:
					wipe.transform.position = Camera.main.transform.position + new Vector3 (0, ((-Screen.height / 64) - 32), 6);
					break;
				}
			}
		}
	}
}
