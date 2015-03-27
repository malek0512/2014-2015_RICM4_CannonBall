#pragma once
#include "mqtt_receiver.h"
#include "mosquitto.h"
#include "AI.h"
#define TOPIC_COORDONNEE	"simulator/coordonnees"
#define TOPIC_ANGLE			"simulator/angle"
class simulator
{
	
public:
	mqtt_receiver *mqtt;
	char* mqtt_host = "localhost";
	int port = 1883;
	//Current state
	int steering = 90;
	int throttle = 91;

	simulator();
	~simulator();
	void main(int argc, char* argv[]);

private:
	void initMQTT();
};

