#include "main_function.h"
#include <stdio.h>
#include <tchar.h>
#include <string>
#include "base64.h"

//Arduino
Serial* arduin;
char* serial_port = "\\\\.\\COM5";

//MQTT
mqtt_sender *sender;
char* mqtt_host;

//Current state
int steering = 90;
int throttle = 91;

//aruco
VideoCapture TheVideoCapturer;
Mat TheInputImage, TheInputImageCopy;
vector<Marker> TheMarkers;
CameraParameters TheCameraParameters;
MarkerDetector MDetector;
Dictionary D;
String TheDict;
String TheCamParam;
float TheMarkerSize;

enum Runmode{ RABBIT, CANNON, MAP };
Runmode run_mode;



/**
* Send steering and throttle angles to the arduino
*/
void sendCommand(Serial* arduin, int steering, int throttle) {
	const int nbByte = 2;

	//Initialazing the buffer
	char bufferW[nbByte] = { (char)(steering), (char)(throttle) };
	if (!(arduin)->WriteData(bufferW, nbByte)) {
		std::cout << "Serial write fail !" << std::endl;
	}

	if (DEBUG) printf("Bytes Written : %d, %d\n", bufferW[0], bufferW[1]);

	//We verify if the bytes written are the ones we send
	char bufferR[nbByte] = { 0, 0 };
	int readResult = 0;
	for (int i = 0; i < nbByte; i++)
		while ((readResult = (arduin)->ReadData(bufferR+i, 1)) == -1);

	if (DEBUG) printf("Bytes Read : %d, %d\n", bufferR[0], bufferR[1]);

	if (!strcmp(bufferW, bufferR))
		printf("ERROR ! (send_command) The bytes we sent doesn't match with the on we have written ! \n");
}

/**
* Initialization of the arduino
*/
void initArduino() {
	arduin = new Serial(serial_port);
	

	if (arduin->IsConnected()) {
		std::cout << "Connection to the arduino OK" << std::endl;

		//Vider le buffer en lecture, pour verification des données ecrites dans le port serial
		int readResult;
		char tmp[1];
		//while ((readResult = (arduin)->ReadData(tmp, 1)) != -1)
		//	if (DEBUG) printf("ARDUINO INIT Bytes Read : readResult(%d), %d \n", readResult, tmp[0]);

		//For leaving the active looping in arduino setup
		char init = 255;
		//(arduin)->WriteData(&init, 1);
		//sendCommand(arduin, steering, throttle); //DO NOT REMOVE THIS LINE FOR SOME REASON IT DOESN't WORK IF YOU DELETE It
	}

}

void initMQTT() {
	std::cout << "Connecting to " << mqtt_host << std::endl;
	sender = new mqtt_sender("sender", mqtt_host, 1883);
	sender->publish_to_mqtt("presence", "Hello mqtt");
}

void initAruco() {
	//Open webcam
	TheVideoCapturer.open(0);
	TheVideoCapturer.set(CV_CAP_PROP_FRAME_WIDTH, 1920 / 3);
	TheVideoCapturer.set(CV_CAP_PROP_FRAME_HEIGHT, 1080 / 3);
	if (!TheVideoCapturer.isOpened()) {
		cerr << "Could not open video" << std::endl;
		exit(-1);
	}

	//Load dictionary
	if (D.fromFile(TheDict) == false) {
		cerr << "Could not open dictionary" << std::endl;
		exit(-1);
	}
	if (D.size() == 0) {
		cerr << "Invalid dictionary" << std::endl;
		exit(-1);
	}
	HighlyReliableMarkers::loadDictionary(D);

	//read first image to get the dimensions
	TheVideoCapturer >> TheInputImage;

	//read camera parameters if passed
	TheCameraParameters.readFromXMLFile(TheCamParam);
	TheCameraParameters.resize(TheInputImage.size());

	//Configure other parameters
	//if (ThePyrDownLevel > 0)
	//	MDetector.pyrDown(ThePyrDownLevel);

	MDetector.setMakerDetectorFunction(aruco::HighlyReliableMarkers::detect);
	MDetector.setThresholdParams(21, 7);
	MDetector.setCornerRefinementMethod(aruco::MarkerDetector::LINES);
	MDetector.setWarpSize((D[0].n() + 2) * 8);
	MDetector.setMinMaxSize(0.005f, 0.5f);

	cv::namedWindow("in", 1);
}



/**
* Send metrics to the MQTT broker
*/
void sendMetrics(int steering, int throttle, double laps, double avg, AccelerometerSensor* accelerometer) {
	sender->publish_to_mqtt(TOPIC_STEER, (char*)std::to_string(steering).c_str());
	sender->publish_to_mqtt(TOPIC_THROT, (char*)std::to_string(throttle).c_str());
	sender->publish_to_mqtt(TOPIC_LAPS, (char*)std::to_string(laps).c_str());
	sender->publish_to_mqtt(TOPIC_AVG, (char*)std::to_string(avg).c_str());
	sender->publish_to_mqtt(TOPIC_ACCELEROMETER, (char*)(
		(std::to_string(accelerometer->getX()) + ":"
		+ std::to_string(accelerometer->getY()) + ":"
		+ std::to_string(accelerometer->getZ())).c_str()
		));
	sender->publish_to_mqtt(TOPIC_NB_MARKERS, (char*)std::to_string(TheMarkers.size()).c_str());
	
	//find the closest marker
	float d = -1;
	for (std::vector<aruco::Marker>::iterator it = TheMarkers.begin(); it != TheMarkers.end(); it++) {
		if (d == -1) {
			d = it->Tvec.ptr<float>(0)[2];
		}
		else if (it->Tvec.ptr<float>(0)[2] < d) {
			d = it->Tvec.ptr<float>(0)[2];
		}
	}

	sender->publish_to_mqtt(TOPIC_CLOSEST, (char*)std::to_string(d).c_str());
}

void updateView() {
	//print marker info and draw the markers in image
	TheInputImage.copyTo(TheInputImageCopy);
	



	vector<unsigned char> buffer;
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compression_params.push_back(1);
	if (!cv::imencode(".jpg", TheInputImageCopy, buffer, compression_params)){
		printf("Image encoding failed");
	}

	//string output = base64_encode(buffer.data(), buffer.size());
	//printf("Output: %s", output.c_str());
	//int msgSize = TheInputImageCopy.total()*TheInputImageCopy.elemSize();
	//sender->publish_to_mqtt("camera/images", (char*) output.c_str());
	//printf("camera/images : size %d, bytes : %02s", msgSize, (char*)TheInputImageCopy.data);




	for (unsigned int i = 0; i < TheMarkers.size(); i++) {
		TheMarkers[i].draw(TheInputImageCopy, Scalar(0, 0, 255), 1);
	}
	cv::imshow("in", TheInputImageCopy);
	cv::waitKey(10);
}

void usage() {
	std::cout << "Usage : dictionary.yml intrinsics.yml marker_size MQTT_host serial_port run_mode <AI params>" << std::endl;
	std::cout << "dictionary.yml : Dictinoary of markers used" << std::endl;
	std::cout << "intrinsics.yml : Camera parameters (calibration)" << std::endl;
	std::cout << "marker_size : size of the markers in meters" << std::endl;
	std::cout << "MQTT_host : address of the MQTT broker" << std::endl;
	std::cout << "serial_port : COM port of the arduino (ex : 'COM5')" << std::endl;
	std::cout << "run mode : either 'rabbit' (follow the marker) or 'cannon' (race)" << std::endl;
	std::cout << "AI params : parameters of the artificial intelligence" << std::endl;
}

void readParams(int argc, char *argv[]) {
	if (argc < 7) {
		usage();
		std::cin.get();
		exit(-1);
	}

	TheDict = argv[1];
	TheCamParam = argv[2];
	TheMarkerSize = (float)atof(argv[3]);
	mqtt_host = argv[4];
	serial_port = argv[5];
	std::string argv6 = argv[6];
	if (argv6 == "rabbit") {
		run_mode = RABBIT;
	}
	else if (argv6 == "cannon") {
		run_mode = CANNON;
	}
	else if (argv6 == "map") {
		run_mode = MAP;
	}
	else {
		usage();
		std::cin.get();
		exit(-1);
	}
}

void choose_run_mode(AI **ia, int argc, char *argv[]) {
	if (run_mode == RABBIT) {
		*ia = new AIRabbit(argc, argv);
		sender->publish_to_mqtt(TOPIC_MODE, "Rabbit");
	}
	else if (run_mode == CANNON) {
		*ia = new AICannonball(argc, argv);
		sender->publish_to_mqtt(TOPIC_MODE, "CannonBall");
	}
	else {
		*ia = new AImap();
		sender->publish_to_mqtt(TOPIC_MODE, "Map");
	}
}

void main_function1(int argc, char *argv[]) {
	readParams(argc, argv);

	//Arduino
	initArduino();

	//MQTT
	mosqpp::lib_init();
	initMQTT();

	//AI
	AI *ai = NULL;
	choose_run_mode(&ai, argc - 7, &(argv[7]));

	//Aruco
	initAruco();

	//Sensor
	AccelerometerSensor accelerometer;

	int index = 0;
	double tick = (double)getTickCount();
	double tick2 = (double)getTickCount();
	double laps = 0;
	double laps2 = 0;
	float total = 0;

	while (arduin->IsConnected()) {
		
		//Calculate processing time
		index++;
		laps = (((double)getTickCount() - tick) / getTickFrequency() * 1000);
		total += (float)laps;
		//std::cout << "time enlapsed : " << laps << " ms" << " (avg : " << total / index << ")" << std::endl;
		tick = (double)getTickCount();

		//Get a new frame
		TheVideoCapturer.retrieve(TheInputImage);
		TheVideoCapturer.grab();

		tick2 = (double)getTickCount();
		//Detection of markers in the image passed
		MDetector.detect(TheInputImage, TheMarkers, TheCameraParameters, TheMarkerSize);
		laps2 = (((double)getTickCount() - tick2) / getTickFrequency() * 1000);
		//std::cout << "time enlasped in detect : " << laps2 << std::endl;

		//Get steering and throttle from AI
		ai->getCommand(&TheMarkers, &steering, &throttle, TheInputImage.size().width);

		//Send command on the serial bus
		//std::cout << "Steering : " << steering << ", Throttle : " << throttle << std::endl;
		if (index == 1) {
			char init = 255;
			(arduin)->WriteData(&init, 1);
		}
		sendCommand(arduin, steering, throttle);

		//Send metrics to the MQTT brocker
		sendMetrics(steering, throttle, laps, total / index, &accelerometer);

		//show input with augmented information
		updateView();
	}

	printf("ERROR ! Arduino is disconnected ! \n");

}