#pragma once
#include "mqtt_receiver.h"
#include "mosquitto.h"
#include "AI.h"
#define SIMULATOR_COORDONNEE	"simulator/coordonnees"
#define SIMULATOR_STEERING		"simulator/steering"
#define SIMULATOR_THROTTLE		"simulator/throttle"
#define SIMULATOR_ANGLE			"simulator/angle"
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

