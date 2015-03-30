#pragma once
//#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <Windows.h>
#include "Serial.h"
#include "aruco.h"
#include "highlyreliablemarkers.h"
#include "cvdrawingutils.h"
#include "AIRabbit.h"
#include "AICannonball.h"
#include "AISheep.h"
#include "AImap.h"
#include "mqtt_sender.h"
#include "mqtt_receiver.h"
#include "AccelerometerSensor.h"

using namespace cv;
using namespace aruco;

#define THE_VIDEO_CAPTURER			0
#define STEERING_STOP				96 //Normalement 90 mais 96 nous semble plus droit 
#define THROTTLE_STOP				91
#define DISABLE_ARDUINO				1
#define DISABLE_MQTT				0
#define DISABLE_ARDUINO_CHECK		1 //Let it disable and it should work 
#define DEBUG_MAIN					1
#define DEBUG						1

enum Runmode{ RABBIT, CANNON, SHEEP, MAP };

class car
{
	
public:
	car();
	~car();
	void main (int argc, char *argv[]);
};





