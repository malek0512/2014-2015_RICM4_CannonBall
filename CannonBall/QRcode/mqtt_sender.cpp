#include <cstdio>
#include <cstring>
#include <iostream>

#include <stdint.h>
#include "mqtt_sender.h"
bool	DEBUG_MQTT		= false;
bool	DISABLE_MQTT	= false;




mqtt_sender::mqtt_sender(const char *id, const char *host, int port) : mosquittopp(id)
{
	 
	this->host = host;
	this->port = port;
	int keepalive = 60;
	bool clean_session = true;

	mosquitto_lib_init();

	//CNX
	this->connect_async(host, port, keepalive);
	
	//LOOP
	loop_start();
	//mosquitto_loop_start(mosq);
	

};

mqtt_sender::~mqtt_sender() {
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}

void mqtt_sender::publish_to_topic(char *topic, char *msg) {
	//if (connected) {
		if (DEBUG_MQTT)
			printf("Publish : TOPIC %s, MSG %s\n", topic, msg);
		//int res = mosquitto_publish(mosq, NULL, topic, strlen(msg)*sizeof(uint8_t), (uint8_t *)msg, 3, false);
		int res = publish(NULL, topic, strlen(msg), msg, 0, false);
		//publish(NULL, topic, strlen(msg), msg, 3, true);
		/* 
		if (res == MOSQ_ERR_SUCCESS ) 
			printf("CAAAAAAAAAAAAAAAAAAAACCCCCCCCCCCCCCCAAAAAAAA\n");
		if (res == MOSQ_ERR_INVAL)
			printf("PIIIIIIIIIIIIIIIIIPPPPPPPPPPPIIIIIIIIIIIIIII\n");
		if (res == MOSQ_ERR_NOMEM)
			printf("WWWWWWWWWWWWWWWWWAZAAAAAAAAAAAAAAAAAAAAAAAA\n");
			*/
	//}

	//else    
	//	fprintf(stderr, "mqtt_sender Error Not connected\n");
}




/* 
void my_message_callback_default(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	fprintf(stderr, "Salut BG ! \n");
	if (message->payloadlen){
		fprintf(stderr, "%s %s\n", message->topic, message->payload);
		//return;
	}
	else
		fprintf(stderr, "%s (null)\n", message->topic);
}

void mqtt_sender::subscribe_to_topic(char* topic)
{
	if (DEBUG_MQTT)
		printf("Subscribe : TOPIC %s\n", topic);

	mosquitto_subscribe(mosq, NULL, topic, 2);
}

void mqtt_sender::subscribe_init() {
	bool clean_session = true;
	int keepalive = 60;
	mosq = mosquitto_new(NULL, clean_session, NULL);
	if (!mosq){
		fprintf(stderr, "mqtt_sender Error: Out of memory.\n");
		return;
	}

	mosquitto_message_callback_set(mosq, my_message_callback_default);


	if (mosquitto_connect(mosq, host, port, keepalive)){
		fprintf(stderr, "mqtt_sender Error Unable to connect.\n");
		return;
	}
	printf("Subscribed \n");
	mosquitto_subscribe(mosq, NULL, "malek", 2);
	mosquitto_publish(mosq, NULL, "malek", strlen("Batista is hereeeeee"), "Batista is hereeeeee", 2, false);

}
void mqtt_sender::set_callback(void callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)) {
	//mosquitto_message_callback_set(mosq, callback);
}

*/
