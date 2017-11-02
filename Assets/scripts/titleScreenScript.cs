using UnityEngine;
using System.Collections;
using UnityEngine.SceneManagement;
using System;
using System.IO;

public class titleScreenScript : MonoBehaviour {

	public GameObject Play;
	public GameObject Quit;

	private GameObject temp;
	private masterscript master;
	private blackWipeTransition trans;
	public AudioSource aud;

	// Use this for initialization
	void Start () {
		temp = GameObject.FindGameObjectWithTag("GameController");
		master = temp.GetComponent<masterscript>();
		trans = master.trans;
	}
		

	// Update is called once per frame
	void Update () {
		
	}
}