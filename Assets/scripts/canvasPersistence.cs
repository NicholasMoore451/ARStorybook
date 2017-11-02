using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class canvasPersistence : MonoBehaviour {

	// Use this for initialization
	void Start () {
		DontDestroyOnLoad (this.gameObject);
		this.GetComponent<Canvas> ().sortingLayerName = "UI Transition";
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
