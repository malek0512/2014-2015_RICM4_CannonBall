#pragma once
#include <stdio.h>
#include "mosquitto.h"
#include <string>
#include <stdint.h>
#include "mqtt_sender.h"

//Constantes de pub/sub
#define TOPIC_STEER				"metrics/steering"
#define TOPIC_THROT				"metrics/throttle"
#define TOPIC_LAPS				"metrics/laps"
#define TOPIC_AVG				"metrics/avg"
#define TOPIC_MODE				"metrics/mode"
#define TOPIC_ACCELEROMETER		"metrics/accelerometer"
#define TOPIC_NB_MARKERS		"metrics/nb_markers"
#define TOPIC_CLOSEST			"metrics/closest"

#define TOPIC_COMMANDS_CAMERA			"camera/commands"
#define TOPIC_COMMANDS_STOP_THE_CAR		"camera/commands"
#define TOPIC_CAMERA_IMAGE				"camera/images"
#define MOSQUITTO_HIGH_PRIORITY			2
#define MOSQUITTO_MIDLE_PRIORITY		1
#define MOSQUITTO_LOW_PRIORITY			0

class mqtt_receiver
{
public:
	
	
	mqtt_receiver(const char *id, const char *host, int port);
	~mqtt_receiver();
	void set_callback(void callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message));
	void publish(char* topic, char* msg);
	void subscribe(char* topic);
	struct mosquitto *mosq = NULL;
	//mqtt_sender sender;
//private:
	
};

