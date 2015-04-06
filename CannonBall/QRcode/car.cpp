#include "car.h"
#include <stdio.h>
#include <string>

bool STOP_THE_CAR = false;
bool STOP_THE_CAMERA = false;
bool cameraEnabled = true;


//Arduino
Serial* arduin;
char* serial_port = "\\\\.\\COM5";

//MQTT
mqtt_sender *sender;
mqtt_receiver *receiver;
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
Runmode run_mode;

//Send steering and throttle angles to the arduino
void sendCommand(Serial* arduin, int steering, int throttle);
//Write for extreme programming buffer is supposed initialized
void writeSerial(Serial* s, char* buffer, unsigned int n);
//Read for extreme programming buffer is supposed allocated
void readSerial(Serial* s, char* buffer, unsigned n);
void choose_run_mode(AI **ia, int argc, char *argv[]);
void initArduino();
void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
void initMQTT();
//Write the piture file to the hard drive
void writeImage();
void initAruco();
//Send metrics to the MQTT broker
void sendMetrics(int steering, int throttle, double laps, double avg, AccelerometerSensor* accelerometer);
void updateView();
void usage();
void readParams(int argc, char *argv[]);

car::car() {};

car::~car() {
	delete arduin;
	delete receiver;
	delete sender;
}

void car::main(int argc, char *argv[]) {
	readParams(argc, argv);

	//Arduino
	initArduino();

	//MQTT
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


	int cpt;
	DWORD deltaTime = 0;
	DWORD lastTime = 0;
	cpt = -1;
	
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
		//MDetector.detect(TheInputImage, TheMarkers, TheCameraParameters, TheMarkerSize);
		laps2 = (((double)getTickCount() - tick2) / getTickFrequency() * 1000);
		//std::cout << "time enlasped in detect : " << laps2 << std::endl;

		//Get steering and throttle from AI
		//ai->getCommand(&TheMarkers, &steering, &throttle, TheInputImage.size().width);

		//Send command on the serial bus
		//std::cout << "Steering : " << steering << ", Throttle : " << throttle << std::endl;
		if (!DISABLE_ARDUINO) {
			if (index == 1) {
				char init = 255;
				(arduin)->WriteData(&init, 1);
			}
		}

		if (!STOP_THE_CAR)
			//sendCommand(arduin, steering, throttle);
			sendCommand(arduin, steering, throttle);
		else
			sendCommand(arduin, STEERING_STOP, THROTTLE_STOP);

		//Send metrics to the MQTT brocker
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
		if (counter > 30)
			printf("%.2f fps\n", fps);
		// overflow protection
		if (counter == (INT_MAX - 1000))
			counter = 0;
		// fps counter end --------------------------------------
		
	}

	printf("ERROR ! Arduino is disconnected ! \n");

}

void sendCommand(Serial* arduin, int steering, int throttle) {
	if (!DISABLE_ARDUINO) {
		const int nbByte = 2;
		//Initialazing the buffer
		char bufferW[nbByte] = { (char)(steering), (char)(throttle) };
		if (!(arduin)->WriteData(bufferW, nbByte)) {
			if (!DISABLE_ARDUINO_CHECK)
				std::cout << "Serial write fail !" << std::endl;
		}

		if (DEBUG_MAIN) printf("Bytes Written : steering, throttle : %d, %d\n", bufferW[0], bufferW[1]);

		if (!DISABLE_ARDUINO_CHECK) {

			//We verify if the bytes written are the ones we sent
			char bufferR[nbByte] = { 0, 0 };
			int readResult = 0;
			for (int i = 0; i < nbByte; i++)
				while ((readResult = (arduin)->ReadData(bufferR + i, 1)) == -1);

			if (DEBUG_MAIN) printf("Bytes Read : %d, %d\n", bufferR[0], bufferR[1]);

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
	if (!DISABLE_ARDUINO) {
		arduin = new Serial(serial_port);

		if (arduin->IsConnected()) {
			std::cout << "Connection to the arduino OK" << std::endl;

			//Vider le buffer en lecture, pour verification des données ecrites dans le port serial
			int readResult;
			char tmp[1];
			//while ((readResult = (arduin)->ReadData(tmp, 1)) != -1)
			//	if (DEBUG_MAIN_MAIN) printf("ARDUINO INIT Bytes Read : readResult(%d), %d \n", readResult, tmp[0]);

			//For leaving the active looping in arduino setup
			//unsigned int emergencyTime = 1000;
			//if (DEBUG_MAIN_MAIN) printf("EmergencyTime : %02x\n", (char*) emergencyTime);
			//writeSerial(arduin, (char*)emergencyTime, 4);
		}
	}
}

void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	if (message->payloadlen){
		if (DEBUG_MAIN)
			fprintf(stderr, "%s %s\n", message->topic, message->payload);

		if (!strcmp(TOPIC_COMMANDS_CAMERA					, message->topic))		STOP_THE_CAMERA = true;
		else if (!strcmp(TOPIC_COMMANDS_STOP_THE_CAR		, message->topic))		STOP_THE_CAR	= true;
		else if (!strcmp("malek",							message->topic))		fprintf(stderr, "%s %s\n", message->topic, message->payload);

		return;
	}
	//Error case
	fprintf(stderr, "%s (null)\n", message->topic);
}

void initMQTT() {
	if (!DISABLE_MQTT) {
		std::cout << "Connecting to " << mqtt_host << std::endl;
		sender = new mqtt_sender("sender_main", mqtt_host, 1883);
		receiver = new mqtt_receiver("receiver_main", mqtt_host, 1883);
		receiver->set_callback(my_message_callback);
		//receiver->envoie();
		//sender->subscribe_init();
		//sender->set_callback(my_message_callback);
	}
}

void writeImage() {
	if (! STOP_THE_CAMERA && ! DISABLE_VIDEO_WEB) {
		vector<unsigned char> buffer;
		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
		compression_params.push_back(9);

		if (!cv::imwrite("../../../camera.jpeg", TheInputImage, compression_params)){
			printf("Image encoding failed\n");
		}

		if (!DISABLE_MQTT) {
			//notify the nodejs server
			sender->publish_to_topic(TOPIC_CAMERA_IMAGE, (char*) "refresh");
		}
	}
}

void initAruco() {
	//Open webcam
	TheVideoCapturer.open(THE_VIDEO_CAPTURER);
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
	MDetector.setMinMaxSize(MIN_QRCODE_DISTANCE_MEAN, 0.5f);

	if (!DISABLE_VIDEO_SCREEN)
		cv::namedWindow("in", 1);
}

void sendMetrics(int steering, int throttle, double laps, double avg, AccelerometerSensor* accelerometer) {
	if (!DISABLE_MQTT) {
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
	if (!DISABLE_VIDEO_SCREEN) {
		//TheInputImage.copyTo(TheInputImageCopy);
		TheInputImageCopy = TheInputImage;

		for (unsigned int i = 0; i < TheMarkers.size(); i++) {
			TheMarkers[i].draw(TheInputImageCopy, Scalar(0, 0, 255), 1);
		}
		cv::imshow("in", TheInputImageCopy);
		cv::waitKey(10);
	}
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
	else if (argv6 == "sheep")
	{
		run_mode = SHEEP;
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
	char* mode;
	if (run_mode == RABBIT) {
		*ia = new AIRabbit(argc, argv);
		mode = "Rabbit" ;
	}
	else if (run_mode == CANNON) {
		*ia = new AICannonball(argc, argv);
		mode = "CannonBall";
	}
	else if (run_mode == SHEEP)
	{
		*ia = new AISheep(argc, argv);
		mode = "Sheep";
	}
	else {
		*ia = new AImap();
		mode = "Map";
	}
	if (!DISABLE_MQTT)
		sender->publish_to_topic(TOPIC_MODE, (char*)mode);
}

