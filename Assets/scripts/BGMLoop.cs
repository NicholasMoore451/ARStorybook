using UnityEngine;
using System.Collections;

public class BGMLoop : MonoBehaviour {

	public AudioSource aud;
	public float loopStart;
	public float loopThresh;
	public float tim; 
	public bool playing;

	// Use this for initialization
	void Start () {

	}
	
	// Update is called once per frame
	void Update () {
		if (playing) {
			if (Time.timeScale != 0) {
				aud.UnPause ();
				tim = aud.time;
				if (!aud.isPlaying) {
					aud.time = loopStart;
					aud.Play ();
				}
			} else {
				aud.Pause ();
			}
		} else {
			aud.Pause ();
		}
	}
}
