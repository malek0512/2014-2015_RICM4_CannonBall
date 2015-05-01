#include "simulator.h"
#include <stdio.h>
#include "AISheep.h"
#include "AIRabbit.h"
#include <string.h>
#include <map>
bool DEBUG_MAIN	= true;

//Méthodes intermédiaires
void my_message_callback_simulator(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
void addQrcode(char *s);
void removeQrcode(char *s);
void updateCar(char* s);
void stringToQrcode(char* s, int *x, int *y, int *id);
QRCode* getNearestQrcode();
float angleNearest(QRCode* nearest, cv::Point2f* voiture);
float qrcodeNearest(QRCode* nearest);
float qrcodeID(QRCode* nearest);

//map<int, QRCode*>* makers = new map<int, QRCode*>();
vector<QRCode*>* makers = new vector<QRCode*>();
cv::Point2f car;


/*
On doit recuperer les donnée du simulateur. 
Les envoyer a l'automate, par un appel a getCommand
Recuperer les intruction pour les réemettre a au simulateur
*/

simulator::simulator() //std::vector<QRCode>* markers)
{
	//this->TheMarkers = markers;
	initMQTT();
	//automata = new AIRabbit();
}


simulator::~simulator()
{
	delete sender;
	delete receiver;
	delete automata;
}

void simulator::main(pugi::xml_document* doc) 
{
	const int argc = 1;
	char* argv[] = { "" };
	automata = new AIRabbit(argc, argv);
	while (true) {
		QRCode* n = getNearestQrcode();
		float angle = angleNearest(n, &car);
		float dist = qrcodeNearest(n);
		int id = (int) qrcodeID(n);
		int steering, throttle;
		((AIRabbit*) automata)->getCommandSimulateur(id, dist, angle, &steering, &throttle);
		
	}
}

//Méthodes intermédiaires

void my_message_callback_simulator(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	if (message->payloadlen){
		if (DEBUG_MAIN)
			fprintf(stderr, "%s %s\n", message->topic, message->payload);

		char* topic = message->topic;
		char* msg = (char*) message->payload;

		if (!strcmp(topic, SIMULATOR_CAR)) {
			updateCar(msg);
		} 
		else if (!strcmp(topic, SIMULATOR_ADD)) {
			addQrcode(msg);
		}
		else if (!strcmp(topic, SIMULATOR_DEL)) {
			removeQrcode(msg);
		}
		else if (!strcmp(topic, SIMULATOR_STEERING)) {

		}
		else if (!strcmp(topic, SIMULATOR_THROTTLE)) {

		}
		else if (!strcmp(topic, SIMULATOR_MOVE)) {
			
		}
		else if (!strcmp(topic, "malek")) {
			fprintf(stderr, "%s %s\n", topic, msg);
		}
		return;
	}
	
	//Error case
	printf("je ne dois pas arriver jusque la !\n");
	fprintf(stderr, "%s (null)\n", message->topic);
}

void simulator::initMQTT() {
	std::cout << "Connecting to " << HOST << std::endl;
	sender = new mqtt_sender("sender", HOST, PORT);
	receiver = new mqtt_receiver("receiver", HOST, PORT);
	receiver->subscribe(SIMULATOR_CAR);
	receiver->subscribe(SIMULATOR_ADD);
	receiver->subscribe(SIMULATOR_DEL);
	receiver->subscribe(SIMULATOR_MOVE);
	receiver->subscribe(SIMULATOR_STEERING);
	receiver->subscribe(SIMULATOR_THROTTLE);

	receiver->set_callback(my_message_callback_simulator);

	//receiver->envoie();
	//sender->subscribe_init();
	//sender->set_callback(my_message_callback);
}

int getID(char* string) {
	//return strtok(string, ":");
	return 0;
}

void stringToQrcode(char* s, float *x, float *y, float *id) {
	char * pch;
	//printf("Splitting string \"%s\" into tokens:\n", s);
	
	pch = strtok(s, ":");
	if (pch != NULL) {
		*x = atof(pch);
		pch = strtok(NULL, ":");
	}
	if (pch != NULL) {
		*y = atof(pch);
		pch = strtok(NULL, ":");
	}
	if (pch != NULL) {
		*id = atof(pch);
	}
}

//map<int, QRCode*> makers = map<int, QRCode*>();

void addQrcode(char *s) {
	float x, y, id;
	stringToQrcode(s,&x,&y,&id);
	//fprintf(stderr, "%s", q->toString());
	fprintf(stderr, "Adding x = %f, y = %f, id = %f\n", x,y,id);
	makers->push_back(new QRCode(x,y,id));
	//makers[((int) id)] = (new QRCode(x, y, id));
}

void removeQrcode(char *s) {
	float x, y, id;
	stringToQrcode(s, &x, &y, &id);
	//fprintf(stderr, "%s", q->toString());
	fprintf(stderr, "Removing x = %f, y = %f, id = %f\n", x, y, id);
	for (unsigned int i = 0; i<makers->size(); ++i)
	{
		QRCode* q = (*makers)[i];
		if (q->id == id) {
			makers->erase(makers->begin() + i);
			break;
		}
	}
}

QRCode* getNearestQrcode() {
	if (makers->size()>0) {
		QRCode* qmin = (*makers)[0];
		double min = qmin->distance(&car);

		for (unsigned int i = 0; i < makers->size(); ++i)
		{
			QRCode* q = (*makers)[i];
			if (q->distance(&car) < min) {
				qmin = q;
			}
		}
		return qmin;
	}
	else
		return NULL;
}

void updateCar(char* s) {
	char * pch;

	pch = strtok(s, ":");
	if (pch != NULL) {
		car.x = atof(pch);
		pch = strtok(NULL, ":");
	}
	if (pch != NULL) {
		car.y = atof(pch);
		pch = strtok(NULL, ":");
	}
}

float angleNearest(QRCode* nearest, cv::Point2f* voiture) {
	cv::Point2f* p = nearest->point;
	float res = p->x - voiture->x;
	if (res < 0) res = -res;
	return res / 250.0;
}

float qrcodeNearest(QRCode* nearest) {
	return nearest->distance(&car);
}

float qrcodeID(QRCode* nearest) {
	return nearest->id;
}

void simulator::sendSimulator(int steering, int throttle) {
	char* msg_steering = new char[100];
	char* msg_throttle = new char[100];
	sprintf(msg_steering, "%d", steering);
	sprintf(msg_throttle, "%d", throttle);
	sender->publish_to_topic(SIMULATOR_THROTTLE, msg_throttle);
	sender->publish_to_topic(SIMULATOR_STEERING, msg_steering);
}