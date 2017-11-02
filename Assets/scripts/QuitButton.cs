using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class QuitButton : MonoBehaviour {

	// Use this for initialization
	void Start () {
		
	}

	void OnMouseDown () {
		Application.Quit ();  // Enter your scene's name here! 
	}

	// Update is called once per frame
	void Update () {
		
	}
}
