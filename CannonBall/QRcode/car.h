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

#define THE_VIDEO_CAPTURER			0 //Malek PC
//#define THE_VIDEO_CAPTURER			2 //Tablette 
#define DISABLE_ARDUINO				1
#define DISABLE_VIDEO_WEB			0 //consomme environ 10 fps : on est 24 sans video_screen
#define DISABLE_MQTT				1
#define DISABLE_ARDUINO_CHECK		1 //Let it disable and it should work 
#define DEBUG_MAIN					0
#define DEBUG						0
#define DISABLE_VIDEO_SCREEN		0 //consomme environ 10 fps

//Optmisation possible pour la distance de Qrcode high permet un meilleur fps, mais reduit la distance du qrcode
#define MIN_QRCODE_DISTANCE_MEAN 0.03f
#define MIN_QRCODE_DISTANCE_HIGH 0.05f
#define MIN_QRCODE_DISTANCE_LOW 0.001f
#define STEERING_STOP				96 //Normalement 90 mais 96 nous semble plus droit 
#define THROTTLE_STOP				91

//parameteres a enlever des arguments 
#define COM "COM4"
#define HOST "localhost"
#define PORT 1883

enum Runmode{ RABBIT, CANNON, SHEEP, MAP };

/* Optimisation 
 * Le set min de la largeur minimale de detection d'un carré influe directement sur la 
 * distance de detection d'un QRCODE puisque il devient plus petit
 */
/* - On a supprimer 2 copie d'image inutile thresh.copyTo(thresh2) voir markerdetector.cpp
 * - enlever l'affichage de la camera en desactivant (mettant a 1) l'option DISABLE_VIDEO_SCREEN ci dessus
 * - On a reduit la liste Qrcode possible dans le fichier passé en parametre 
 */
class car
{
	
public:
	car();
	~car();
	void main (int argc, char *argv[]);
};





