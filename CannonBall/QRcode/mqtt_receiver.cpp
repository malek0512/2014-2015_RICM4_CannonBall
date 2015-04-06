#include "mqtt_receiver.h"

/*
A l'egard de la personne chanceuse qui aura remarquer ce commentaire ne faite pas de publish avec un QOS de 2 car les 
message ne sont pas publiés par le borker. Je PENSE que ca doit venir du fait qu'une qualité de service maximale exige des ACK
qui ne serait jamais émis ~~ 
*/
void my_message_callback_default(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	fprintf(stderr, "my_message_callback BRO ! \n");

	if (message->payloadlen){
		fprintf(stderr, "%s %s\n", message->topic, message->payload);
		
	}
	else{
		fprintf(stderr, "%s (null)\n", message->topic);
	}
	//fflush(stdout);
}

mqtt_receiver::mqtt_receiver(const char *id, const char *host, int port)
{
	//sender = new mqtt_sender("sender", host, port);
	int keepalive = 60;
	bool clean_session = true;
		

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, clean_session, NULL);
	if (!mosq){
		fprintf(stderr, "Error: Out of memory.\n");
		//return 1;
		throw 1;
	}

	mosquitto_message_callback_set(mosq, my_message_callback_default);

	if (mosquitto_connect(mosq, host, port, keepalive)){
		fprintf(stderr, "Unable to connect.\n");
		throw 1;
	}


	mosquitto_subscribe(mosq, NULL, "malek", 2);
		
	mosquitto_loop_start(mosq);
}



mqtt_receiver::~mqtt_receiver()
{
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	//delete mosq;
}

void mqtt_receiver::publish(char* topic, char* msg) {
	//mosquitto_publish(mosq, NULL, "malek", strlen("WAZAZAZA"), "WAZAZAZA", 2, false);
	mosquitto_publish(mosq, NULL, topic, strlen(msg), msg, 0, true);
}

void mqtt_receiver::set_callback(void callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)) {
	mosquitto_message_callback_set(mosq, callback);
}

void mqtt_receiver::subscribe(char* topic) {
	mosquitto_subscribe(mosq, NULL, topic, 2);
}