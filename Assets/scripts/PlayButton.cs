using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayButton : MonoBehaviour {

	private GameObject temp;
	private masterscript master;
	private blackWipeTransition trans;

	// Use this for initialization
	void Start () {
		temp = GameObject.FindGameObjectWithTag("GameController");
		master = temp.GetComponent<masterscript>();
		trans = master.trans;
	}

	void OnMouseDown () {
		trans.LoadScene (1, "");  // Enter your scene's name here! 
	}

	// Update is called once per frame
	void Update () {
		
	}
}
