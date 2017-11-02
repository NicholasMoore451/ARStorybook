using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class clickthrough : MonoBehaviour {

	public blackWipeTransition trans;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		if (Input.anyKeyDown) {
			trans.LoadScene (0, "titleScreen");
		}
	}
}
