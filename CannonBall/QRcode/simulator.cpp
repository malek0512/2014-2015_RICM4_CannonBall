#include "simulator.h"
#include <stdio.h>
#include "AISheep.h"
#include "AIRabbit.h"
#include <string.h>

bool DEBUG_MAIN	= true;

//Méthodes intermédiaires
void my_message_callback_simulator(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
void addQrcode(char *s);
void removeQrcode(char *s);
void updateCar(char* s);
void stringToQrcode(char* s, int *x, int *y, int *id);
std::vector<QRCode*>* markers = new vector<QRCode*>();

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

void simulator::main(int argc, char* argv[]) 
{
	//simulator simulator = new simulator(markers);
	automata = new AIRabbit(argc, argv);
	while (true) {

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

		if (!strcmp(topic, SIMULATOR_ANGLE)) {
			//qrcode[]
		} 
		else if (!strcmp(topic, SIMULATOR_COORDONNEE)) {
			addQrcode(msg);
			
		}
		else if (!strcmp(topic, SIMULATOR_THROTTLE)) {

		}
		else if (!strcmp(topic, SIMULATOR_STEERING)) {

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
	std::cout << "Connecting to " << mqtt_host << std::endl;
	sender = new mqtt_sender("sender", mqtt_host, port);
	receiver = new mqtt_receiver("receiver", mqtt_host, port);
	receiver->subscribe(SIMULATOR_COORDONNEE);
	receiver->subscribe(SIMULATOR_ANGLE);
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

void addQrcode(char *s) {
	float x, y, id;
	stringToQrcode(s,&x,&y,&id);
	//fprintf(stderr, "%s", q->toString());
	fprintf(stderr, "x = %f, y = %f, id = %f\n", x,y,id);
	markers->push_back(new QRCode(x,y,id));
}

void removeQrcode(char *s) {
	//TODO
}

void updateCar(char* s) {
	//TODO
}

