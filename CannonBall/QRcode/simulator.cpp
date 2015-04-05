#include "simulator.h"
#include <stdio.h>
#include "AISheep.h"
#include "AIRabbit.h"
#include <string.h>

bool DEBUG_MAIN	= true;

//Méthodes intermédiaires
void my_message_callback_simulator(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);

/*
On doit recuperer les donnée du simulateur. 
Les envoyer a l'automate, par un appel a getCommand
Recuperer les intruction pour les réemettre a au simulateur
*/

simulator::simulator()
{
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
	automata = new AIRabbit(argc, argv);
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
	sender = new mqtt_sender("sender", mqtt_host, 1883);
	receiver = new mqtt_receiver("receiver", mqtt_host, 1883);
	receiver->set_callback(my_message_callback_simulator);
	//receiver->envoie();
	//sender->subscribe_init();
	//sender->set_callback(my_message_callback);
}

int getID(char* string) {
	//return strtok(string, ":");
	return 0;
}