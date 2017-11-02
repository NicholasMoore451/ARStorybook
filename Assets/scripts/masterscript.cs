using UnityEngine;
using System.Collections;
using UnityEngine.SceneManagement;

public class masterscript : MonoBehaviour {


	public blackWipeTransition trans;

	// Use this for initialization
	void Start () {
		DontDestroyOnLoad (this.gameObject); // this gameObject won't be destroyed when the scene changes
		Cursor.visible = true; // sets cursor visibility
	}

	// Update is called once per frame
	void Update () {

	}

}
