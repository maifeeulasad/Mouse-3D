using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;

public class Control : MonoBehaviour
{
    public bool pos = false;

    //Transform position modifier
    public float posMod = 10f;
    readonly int translationDamping = 10;
    public int[] serPos = new int[] { 0, 2, 1 };
    public bool[] dirPos = new bool[] { false, false, false };
    private float posX = 0, posY = 0, posZ = 0;
    private float[] vel = new float[] { 0f, 0f, 0f };



    //Streaming 
    readonly static string portName = "COM3";
    readonly static int baudRate = 115200;
    SerialPort stream = new SerialPort(portName, baudRate);

    //Accelerometer & Gyroscopic data
    //a - acceleration & t - theta
    public static float[] a = new float[] { 0, 0, 0 };
    public static float[] t = new float[] { 0, 0, 0 };

    //Rotaion modifier
    readonly float[] offsetRot = new float[] { 0f, 0f, 0f };
    readonly int[] serRot = new int[] { 0, 2, 1 };
    readonly bool[] dirRot = new bool[] { false, false, false };
    readonly float rotationalDamping = 1;

    //Opens stream on awake, makes unity faster
    void Awake()
    {
        stream.Open();
    }

    //Closes stream so next time it doesn't create problems 
    void OnDestroy()
    {
        stream.Close();
    }

    //Recieves streams, process them, set them, flush them
    void Update()
    {
        try
        {
            string value = stream.ReadLine();
            string[] rot = value.Split(':');
            
            float tX, tY, tZ;
            tX = float.Parse(rot[0]);
            tY = float.Parse(rot[1]);
            tZ = float.Parse(rot[2]);
            if(!NearlyEqual(tX, t[0]) && !NearlyEqual(tY, t[1]) && !NearlyEqual(tZ, t[2]))
            {
                a[0] = float.Parse(rot[3]);
                a[1] = float.Parse(rot[4]);
                a[2] = float.Parse(rot[5]);
                Debug.Log(a[0] + ":" + a[1] + ":" + a[2]);
            }
            else
            {
                a[0] = a[1] = a[2] = 0;
                vel[0] = vel[1] = vel[2] = 0;
            }

            t[0] = tX;
            t[1] = tY;
            t[2] = tZ;
            
            stream.BaseStream.Flush();

            Quaternion targetRotation = AngleQ();
            transform.rotation = Quaternion.Lerp(transform.rotation, targetRotation, rotationalDamping);

            if(pos)
            {
                Vector3 v = Vector3.zero;
                transform.position = Vector3.SmoothDamp(transform.position, Position(),ref v, Time.deltaTime * translationDamping);

            }
        }
        catch (Exception e)
        {
            throw e;
        }
    }

    Quaternion AngleQ()
    {
        return Quaternion.Euler(AngleV());
    }

    Vector3 AngleV()
    {
        return new Vector3(TX(), TY(), TZ());
    }

    float TX()
    {
        return BoolToSign(dirRot[serRot[0]]) * (t[serRot[0]] + offsetRot[serRot[0]]);
    }

    float TY()
    {
        return BoolToSign(dirRot[serRot[1]]) * (t[serRot[1]] + offsetRot[serRot[1]]);
    }

    float TZ()
    {
        return BoolToSign(dirRot[serRot[2]]) * (t[serRot[2]] + offsetRot[serRot[2]]);
    }

    int BoolToSign(bool op)
    {
        if (op)
            return 1;
        return -1;
    }
    float PX()
    {
        if (a[serPos[0]] < 0.1f && vel[serPos[0]] < 0.1f)
            return posX;
        float u = vel[serPos[0]];
        vel[serPos[0]] += a[serPos[0]] * Time.deltaTime;
        float v = vel[serPos[0]];
        try
        {
            posX += (BoolToSign(dirPos[serPos[0]]) * (v * v - u * u) / 2 / a[serPos[0]]) * posMod;
        }
        catch(Exception e)
        {

        }
        return posX;
    }
    float PY()
    {
        if (a[serPos[1]] < 0.1f && vel[serPos[1]] < 0.1f)
            return posY;
        float u = vel[serPos[1]];
        vel[serPos[1]] += a[serPos[1]] * Time.deltaTime;
        float v = vel[serPos[1]];
        try
        {
            posY += (BoolToSign(dirPos[serPos[1]]) * (v * v - u * u) / 2 / a[serPos[1]]) * posMod;
        }
        catch (Exception e)
        {

        }
        return posY;
    }
    float PZ()
    {

        if (a[serPos[2]] < 0.1f && vel[serPos[2]] < 0.1f)
            return posZ;
        float u = vel[serPos[2]];
        vel[serPos[2]] += a[serPos[2]] * Time.deltaTime;
        float v = vel[serPos[2]];
        try
        {
            posZ += (BoolToSign(dirPos[serPos[2]]) * (v * v - u * u) / 2 / a[serPos[2]]) * posMod;
        }
        catch (Exception e)
        {

        }
        return posZ;
    }

    Vector3 Position()
    {
        Vector3 tem = new Vector3(PX(), PY(), PZ());
        //Debug.Log(tem);
        return tem;
    }

    public bool NearlyEqual(float a, float b)
    {
        float epsilon = 1e-1f;
        return b - epsilon < a && a < b + epsilon;
    }
}