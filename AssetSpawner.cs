using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//Add this to any object that will spawn prefabs. 
//Make sure prefab is in the resources folder
//Resources folder is in the root of the project
public class AssetSpawner : MonoBehaviour {

    public Vector3 position;
    public float spawnDelay;
    public float nextSpawnTime

    void Start()
    {

    }

    void Update()
    {
        //Can create prefab on command
        if (Input.GetKeyDown("s"))
        {
            createPreFab();
        }
        //Can create prefab on timer
        //if (checkTimer())
        //{
        //  createPreFabOnTimer();   
        //}
        
    }

    void createPreFab()
    {
        GameObject myPreFabClone = Resources.Load("Box") as GameObject;
        Instantiate(myPreFabClone, position, Quaternion.identity);
    }
   
    void createPreFabOnTimer()
    {
        nextSpawnTime = Time.time + spawnDelay;
        createPreFab();
    }
    
    bool checkTimer()
    {
        return Time.time > nextSpawnTime;
    }
    
    
}
