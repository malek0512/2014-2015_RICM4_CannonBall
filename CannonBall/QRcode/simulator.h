#pragma once
#include "mqtt_receiver.h"
#include "mosquitto.h"
#include "AI.h"
#define TOPIC_COORDONNEE	"simulator/coordonnees"
#define TOPIC_STEERING		"simulator/steering"
#define TOPIC_THROTTLE		"simulator/throttle"
#define TOPIC_ANGLE			"simulator/angle"
class simulator
{
	
public:
	//mqtt_receiver *mqtt;
	
	mqtt_receiver *receiver;
	mqtt_sender *sender;
	int port = 1883;
	char* mqtt_host = "localhost";

	//automata
	AI* automata;

	//Current state
	int steering = 90;
	int throttle = 91;

	simulator();
	~simulator();
	void main(int argc, char* argv[]);

private:
	void initMQTT();
};

