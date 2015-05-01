#include "car.h"
#include "main.h"
#include <stdio.h>
#include <string>

#define PRINTER(name) #name

bool STOP_THE_CAR = false;
bool STOP_THE_CAMERA = false;
bool cameraEnabled = true;

float MinQrcodeDistance;
int video_capurer;
char* serial;
char* host;
int port;

//parameteres a enlever des arguments 
int DisableMicroController;
int DisableMicroControllerCheck;		//Let it disable and it should work 
int DisableDebug;
int DisableSimulator;			//consomme environ 10 fps
int DisableFps;
int DisableMosquitto;
int DisableMetrics;
int DisableVideoServer; //consomme environ 10 fps : on est 24 sans video_screen
int DisableVideoScreen;		//consomme environ 10 fps

//Arduino
Serial* arduin;

//MQTT
mqtt_sender *sender;
mqtt_receiver *receiver;


//Current state
int steering = 90;
int throttle = 91;
int tmp_steering = 0;
int tmp_throttle = 0;

//aruco
VideoCapture TheVideoCapturer;
Mat TheInputImage, TheInputImageCopy;
vector<Marker> TheMarkers;
CameraParameters TheCameraParameters;
MarkerDetector MDetector;
Dictionary D;
String dictionary;
String camera;
float marker_size;
Runmode run_mode;

//Send steering and throttle angles to the arduino
void sendCommand(Serial* arduin, int steering, int throttle);
//Write for extreme programming buffer is supposed initialized
void writeSerial(Serial* s, char* buffer, unsigned int n);
//Read for extreme programming buffer is supposed allocated
void readSerial(Serial* s, char* buffer, unsigned n);
void initArduino();
void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
void initMQTT();
//Write the piture file to the hard drive
void writeImage();
void initAruco();
//Send metrics to the MQTT broker
void sendMetrics(int steering, int throttle, double laps, double avg, AccelerometerSensor* accelerometer);
void updateView();
void readParams(AI **ia, pugi::xml_document* doc);
void sendSimulator(int *tmp_throttle, int *tmp_steering, int steering, int throttle);

car::car() {};

car::~car() {
	delete arduin;
	delete receiver;
	delete sender;
}

void car::main(pugi::xml_document* doc) {
	AI *ai = NULL;
	readParams(&ai, doc);

	//Arduino
	initArduino();

	//MQTT
	initMQTT();

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


	//int cpt;
	//DWORD deltaTime = 0;
	//DWORD lastTime = 0;
	//cpt = -1;
	
	time_t start, end;
	int counter = 0;
	double sec;
	double fps;
	//cv::namedWindow("in", 1);
	
	while (true) {
		
		// fps counter begin ------------------------------------
		if (counter == 0){
			time(&start);
		}
		// fps counter end ---------------------------------------

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
		MDetector.detect(TheInputImage, TheMarkers, TheCameraParameters, marker_size);
		laps2 = (((double)getTickCount() - tick2) / getTickFrequency() * 1000);
		//std::cout << "time enlasped in detect : " << laps2 << std::endl;

		//Get steering and throttle from AI
		ai->getCommand(&TheMarkers, &steering, &throttle, TheInputImage.size().width);

		//Send command on the serial bus
		//std::cout << "Steering : " << steering << ", Throttle : " << throttle << std::endl;
		if (!DisableMicroController) {
			if (index == 1) {
				char init = 255;
				(arduin)->WriteData(&init, 1);
			}
		}

		if (!STOP_THE_CAR)
			sendCommand(arduin, steering, throttle);
		else
			sendCommand(arduin, STEERING_STOP, THROTTLE_STOP);

		sendSimulator(&tmp_throttle, &tmp_steering, steering, throttle);
		

		//Send metrics to the MQTT brocker
		if (!DisableMetrics) 
			sendMetrics(steering, throttle, laps, total / index, &accelerometer);

		//show input with augmented information
		updateView();

		//write the image
		writeImage();

		// fps counter begin -------------------------------------
		time(&end);
		counter++;
		sec = difftime(end, start);
		fps = counter / sec;
		if ( ! DisableFps && counter > 30)
			printf("%.2f fps\n", fps);
		// overflow protection
		if (counter == (INT_MAX - 1000))
			counter = 0;
		// fps counter end --------------------------------------
		
	}

	printf("ERROR ! Arduino is disconnected ! \n");

}

void sendCommand(Serial* arduin, int steering, int throttle) {
	if (!DisableMicroController) {
		const int nbByte = 2;
		//Initialazing the buffer
		char bufferW[nbByte] = { (char)(steering), (char)(throttle) };
		if (!(arduin)->WriteData(bufferW, nbByte)) {
			if (!DisableMicroControllerCheck)
				std::cout << "Serial write fail !" << std::endl;
		}

		if (! DisableDebug) printf("Bytes Written : steering, throttle : %d, %d\n", bufferW[0], bufferW[1]);

		if (!DisableMicroControllerCheck) {

			//We verify if the bytes written are the ones we sent
			char bufferR[nbByte] = { 0, 0 };
			int readResult = 0;
			for (int i = 0; i < nbByte; i++)
				while ((readResult = (arduin)->ReadData(bufferR + i, 1)) == -1);

			if (! DisableDebug) printf("Bytes Read : %d, %d\n", bufferR[0], bufferR[1]);

			if (!strcmp(bufferW, bufferR))
				printf("ERROR ! (send_command) The bytes we sent doesn't match with the on we have written ! \n");
		}
	}
	
}

void writeSerial(Serial* s, char* buffer, unsigned int n) {
	int res = 0;
	char b[1] = { 0 };
	for (int i = 0; i < n; i++) {
		s->WriteData(buffer+i, n);
		
		if ((res = s->ReadData(b, 1)) != 1 || b[0] != buffer[i])
			i--;
	}
}

void readSerial(Serial* s, char* buffer, unsigned n) {
	for (int i = 0; i < n; i++) {
		while (s->ReadData(buffer+i, 1) == -1);
	}
}

void initArduino() {
	if (!DisableMicroController) {
		arduin = new Serial(serial);

		if (arduin->IsConnected()) {
			std::cout << "Connection to the arduino OK" << std::endl;

			//Vider le buffer en lecture, pour verification des données ecrites dans le port serial
			int readResult;
			char tmp[1];
			//while ((readResult = (arduin)->ReadData(tmp, 1)) != -1)
			//	if (! DisableDebug) printf("ARDUINO INIT Bytes Read : readResult(%d), %d \n", readResult, tmp[0]);

			//For leaving the active looping in arduino setup
			//unsigned int emergencyTime = 1000;
			//if (! DisableDebug) printf("EmergencyTime : %02x\n", (char*) emergencyTime);
			//writeSerial(arduin, (char*)emergencyTime, 4);
		}
	}
}

void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	if (message->payloadlen){
		if (! DisableDebug)
			fprintf(stderr, "%s %s\n", message->topic, message->payload);

		if (!strcmp(TOPIC_COMMANDS_CAMERA					, message->topic))		STOP_THE_CAMERA = true;
		else if (!strcmp(TOPIC_COMMANDS_STOP_THE_CAR		, message->topic))		STOP_THE_CAR	= true;
		else if (!strcmp("simulator/car", message->topic)){
			//sendSimulator(steering, throttle);
		}
		else if (!strcmp("malek",							message->topic))		fprintf(stderr, "%s %s\n", message->topic, message->payload);

		return;
	}
	//Error case
	fprintf(stderr, "%s (null)\n", message->topic);
}

void initMQTT() {
	if (!DisableMosquitto) {
		std::cout << "Connecting to " << host << std::endl;
		sender = new mqtt_sender("sender_main", host, port);
		receiver = new mqtt_receiver("receiver_main", host, port);
		receiver->set_callback(my_message_callback);
		receiver->subscribe("simulator/car");
		//receiver->envoie();
		//sender->subscribe_init();
		//sender->set_callback(my_message_callback);
	}
}

void writeImage() {
	if (! STOP_THE_CAMERA && ! DisableVideoServer) {
		vector<unsigned char> buffer;
		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
		compression_params.push_back(9);

		if (!cv::imwrite("../../../camera.jpeg", TheInputImage, compression_params)){
			printf("Image encoding failed\n");
		}

		if (!DisableMosquitto) {
			//notify the nodejs server
			sender->publish_to_topic(TOPIC_CAMERA_IMAGE, (char*) "refresh");
		}
	}
}

void initAruco() {
	//Open webcam
	TheVideoCapturer.open(video_capurer);
	TheVideoCapturer.set(CV_CAP_PROP_FRAME_WIDTH, 1920 / 3);
	TheVideoCapturer.set(CV_CAP_PROP_FRAME_HEIGHT, 1080 / 3);
	if (!TheVideoCapturer.isOpened()) {
		cerr << "Could not open video" << std::endl;
		exit(-1);
	}

	//Load dictionary
	if (D.fromFile(dictionary) == false) {
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
	TheCameraParameters.readFromXMLFile(camera);
	TheCameraParameters.resize(TheInputImage.size());

	//Configure other parameters
	//if (ThePyrDownLevel > 0)
	//	MDetector.pyrDown(ThePyrDownLevel);

	MDetector.setMakerDetectorFunction(aruco::HighlyReliableMarkers::detect);
	MDetector.setThresholdParams(21, 7);
	MDetector.setCornerRefinementMethod(aruco::MarkerDetector::LINES);
	MDetector.setWarpSize((D[0].n() + 2) * 8);
	MDetector.setMinMaxSize(MinQrcodeDistance, 0.5f);

	if (!DisableVideoScreen)
		cv::namedWindow("in", 1);
}

void sendMetrics(int steering, int throttle, double laps, double avg, AccelerometerSensor* accelerometer) {
	if (!DisableMosquitto) {
		sender->publish_to_topic(TOPIC_STEER, (char*)std::to_string(steering).c_str());
		sender->publish_to_topic(TOPIC_THROT, (char*)std::to_string(throttle).c_str());
		sender->publish_to_topic(TOPIC_LAPS, (char*)std::to_string(laps).c_str());
		sender->publish_to_topic(TOPIC_AVG, (char*)std::to_string(avg).c_str());
		sender->publish_to_topic(TOPIC_ACCELEROMETER, (char*)(
			(std::to_string(accelerometer->getX()) + ":"
			+ std::to_string(accelerometer->getY()) + ":"
			+ std::to_string(accelerometer->getZ())).c_str()
			));
		sender->publish_to_topic(TOPIC_NB_MARKERS, (char*)std::to_string(TheMarkers.size()).c_str());

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

		sender->publish_to_topic(TOPIC_CLOSEST, (char*)std::to_string(d).c_str());
	}
}

void updateView() {
	//cv::namedWindow("in", 1);
	//print marker info and draw the markers in image
	if (!DisableVideoScreen) {
		//TheInputImage.copyTo(TheInputImageCopy);
		TheInputImageCopy = TheInputImage;

		for (unsigned int i = 0; i < TheMarkers.size(); i++) {
			TheMarkers[i].draw(TheInputImageCopy, Scalar(0, 0, 255), 1);
		}
		cv::imshow("in", TheInputImageCopy);
		cv::waitKey(10);
	}
}


void readParams(AI** ia, pugi::xml_document* doc) {
	char* mode = (char*) doc->child("Run").attribute("mode").as_string();
	pugi::xml_node args = doc->child("Arguments").find_child_by_attribute("mode", mode);
	pugi::xml_node opts = doc->child("Optimisations");
	
	//dictionary = args.child("Aruco").attribute("dictionary").as_string();
	//camera = args.child("Opencv").attribute("camera").as_string();
	//marker_size = args.child("Aruco").attribute("marker_size").as_float();
	//host = (char*) args.child("Mosquitto").attribute("host").as_string();
	//port = args.child("Mosquitto").attribute("port").as_int();
	//serial = (char*) args.child("MicroController").attribute("serial").as_string();
	//video_capurer = args.child("Opencv").attribute("video_capturer").as_int();
	//MinQrcodeDistance = opts.child("MinQrcodeDistance").attribute("value").as_float();

	//DisableDebug = opts.child("DisableDebug").attribute("value").as_int();
	//DisableMicroController = opts.child("DisableMicroController").attribute("value").as_int();
	//DisableMicroControllerCheck = opts.child("DisableMicroControllerCheck").attribute("value").as_int();
	//DisableSimulator = opts.child("DisableSimulator").attribute("value").as_int();
	//DisableMosquitto = opts.child("DisableMosquitto").attribute("value").as_int();
	//DisableMetrics = opts.child("DisableMetrics").attribute("value").as_int();
	//DisableFps = opts.child("DisableFps").attribute("value").as_int();
	//DisableVideoServer = opts.child("DisableVideoServer").attribute("value").as_int();
	//DisableVideoScreen = opts.child("DisableVideoScreen").attribute("value").as_int();


	dictionary =						Variables::getValueAsString(PRINTER(dictionary));
	camera =							Variables::getValueAsString(PRINTER(camera));
	marker_size =						Variables::getValueAsInt(PRINTER(marker_size));
	host =								Variables::getValueAsString(PRINTER(host));
	port =								Variables::getValueAsInt(PRINTER(port));
	serial =							Variables::getValueAsString(PRINTER(serial));
	video_capurer =						Variables::getValueAsInt(PRINTER(video_capurer));
	MinQrcodeDistance =					Variables::getValueAsFloat(PRINTER(MinQrcodeDistance));

	DisableDebug =						Variables::getValueAsInt(PRINTER(DisableDebug));
	DisableMicroController =			Variables::getValueAsInt(PRINTER(DisableMicroController));
	DisableMicroControllerCheck =		Variables::getValueAsInt(PRINTER(DisableMicroControllerCheck));
	DisableSimulator =					Variables::getValueAsInt(PRINTER(DisableSimulator));
	DisableMosquitto =					Variables::getValueAsInt(PRINTER(DisableMosquitto));
	DisableMetrics =					Variables::getValueAsInt(PRINTER(DisableMetrics));
	DisableFps =						Variables::getValueAsInt(PRINTER(DisableFps));
	DisableVideoServer =				Variables::getValueAsInt(PRINTER(DisableVideoServer));
	DisableVideoScreen =				Variables::getValueAsInt(PRINTER(DisableVideoScreen));

	const int argc = 1;
	char* argv[argc];
	if (!strcmp(mode, "Rabbit")) {
		argv[0] = (char*) args.child("Aruco").attribute("qrcode_id").as_string();
		*ia = new AIRabbit(argc, argv);
	}
	else if (!strcmp(mode, "Cannon")) {
		argv[0] = (char*)args.child("Aruco").attribute("file").as_string();
		*ia = new AICannonball(argc, argv);
	}
	else if (!strcmp(mode, "Sheep")) {
		argv[0] = (char*)args.child("Config").attribute("file").as_string();
		*ia = new AISheep(argc, argv);
	}
	else if (!strcmp(mode, "Map")) {
		*ia = new AImap();
	} 
	else {
		std::cout << "Unknown mode\n" << std::endl;
		exit(-1);
	}

	if (!DisableMosquitto)
		sender->publish_to_topic(TOPIC_MODE, (char*)mode);
}


void sendSimulator(int *tmp_throttle, int *tmp_steering, int steering, int throttle) {
	if (! DisableSimulator) {
		char* msg_steering = new char[10];
		char* msg_throttle = new char[10];
		//fprintf(stderr, "throttle %d, tmp:%d   sterring:%d,tmp:%d\n", throttle, *tmp_throttle, steering, *tmp_steering);
		if (*tmp_throttle != throttle || *tmp_steering != steering) {
			sprintf(msg_throttle, "%d", throttle);

			fprintf(stderr, "simulator : sending throttle  : %s\n", msg_throttle);
			sender->publish_to_topic(SIMULATOR_THROTTLE, msg_throttle);
			*tmp_throttle = throttle;

			sprintf(msg_steering, "%d", steering);
			fprintf(stderr, "simulator : sending steering  : %s\n", msg_steering);
			sender->publish_to_topic(SIMULATOR_STEERING, msg_steering);
			*tmp_steering = steering;
		}
	}
}