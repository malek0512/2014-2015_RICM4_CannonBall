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
#include "AccelerometerSensor.h"

using namespace cv;
using namespace aruco;

static const boolean DEBUG = true;

void main_function1(int argc, char *argv[]);