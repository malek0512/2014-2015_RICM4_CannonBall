#include <cstdio>
#include <cstring>
#include <iostream>

#include <stdint.h>
#include "mqtt_sender.h"

mqtt_sender::mqtt_sender(const char *id, const char *host, int port) : mosquittopp(id)
{
    int keepalive = 60;
    connected = false;

    /* Connect immediately. This could also be done by calling
     * mqtt_sender->connect(). */
	connected = (connect_async(host, port, keepalive) == MOSQ_ERR_SUCCESS);
	mosquitto_loop_start(NULL);
    //loop_start();
};

void mqtt_sender::publish_to_mqtt(char *topic, char *msg) {
	//printf("NC Publish : TOPIC %s, MSG %s\n", topic, msg);
	if (connected) {
		printf("Publish : TOPIC %s, MSG %s\n", topic, msg);
		publish(NULL, topic, strlen(msg)*sizeof(uint8_t), (uint8_t *)msg); //Client
		
		int id = 42;
		//mosquitto_publish(NULL, &id, topic, strlen(msg), msg, 0, true);
	}
        
}

void mqtt_sender::subscribe_to_mqtt(char *topic) {
	//printf("NC Publish : TOPIC %s, MSG %s\n", topic, msg);
	if (connected) {
		printf("Subscribe : TOPIC %s\n", topic);
		subscribe(NULL, topic, 2);
	}
}

void mqtt_sender::on_message(mosquitto_message* msg) {
	
	printf("******************************* Received msg : %s *****************************************************\n", msg->topic);
}

/*
void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
	if (message->payloadlen){
		printf("%s %s\n", message->topic, message->payload);
	}
	else{
		printf("%s (null)\n", message->topic);
	}
	fflush(stdout);
}


std::string* mqtt_sender::on_message_of_mqtt(char *topic) {
	//printf("NC Publish : TOPIC %s, MSG %s\n", topic, msg);
	printf("ON_MESSAGE : TOPIC %s\n", topic);
	if (connected) {
		printf("CONNECTED\n");
		//printf("Subscribe : TOPIC %s\n", topic);
		//const mosquitto_message* msg = new mosquitto_message();
		//on_message(msg);
		try {
			mosquitto_message_callback_set((mosquitto*) this, my_message_callback);
		}
		catch (std::exception e) {
			printf("CallBack on_message_of_mqtt excception : %s\n", e.what());
		}
		
		
		/*
		printf("MSG RECU : TOPIC %s, MSG : %s \n", topic, (char*) msg->payload);
		if (! strcmp(msg->topic, TOPIC_CAMERA_COMMANDS)) {
			const char* r = (char*) msg->payload;
			std::string* ret = new std::string(r);
			printf("MSG RECU : TOPIC %s, MSG : %s \n", topic, r);
			return ret;
		}
		
	}
	return NULL;
}

*/

