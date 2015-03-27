#include "simulator.h"


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
	//initMQTT();

}


simulator::~simulator()
{
	//delete mqtt;
	//delete automata;
}

void simulator::main(int argc, char* argv[]) 
{

}

//Méthodes intermédiaires

void my_message_callback_simulator(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	if (message->payloadlen){
		if (DEBUG_MAIN)
			fprintf(stderr, "%s %s\n", message->topic, message->payload);

		char* topic = message->topic;
		char* msg = (char*) message->payload;

		if (!strcmp(topic, TOPIC_ANGLE)) {

		} else if (!strcmp(topic, TOPIC_COORDONNEE)) {

		} else if (!strcmp(topic, "malek")) {
			fprintf(stderr, "%s %s\n", topic, msg);
		}
		return;
	}
	
	//Error case
	printf("je ne dois pas arriver jusque la !\n");
	fprintf(stderr, "%s (null)\n", message->topic);
}

void simulator::initMQTT() {
	printf("Connecting to %s", mqtt_host);
	mqtt = new mqtt_receiver("receiver", mqtt_host, port);
	mqtt->set_callback(my_message_callback_simulator);
}