#pragma once
#include "mqtt_receiver.h"
#include "mosquitto.h"
#include "AI.h"
#include <map>
#define SIMULATOR_COORDONNEE	"simulator/coordonnees"
#define SIMULATOR_STEERING		"simulator/steering"
#define SIMULATOR_THROTTLE		"simulator/throttle"
#define SIMULATOR_ANGLE			"simulator/angle"

/*FORMAT COORDONNEES <ID:x,y>*/
class QRCode {
public :
	int x, y, id;
};

class simulator
{
	
public:
	//mqtt_receiver *mqtt;
	std::map<int, QRCode> qrcode;
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

/*
Ce qui est imortant : 
- gerer la cllision et sortir d'ecran c'est lunivers qui se deplace soit thore
- envoyer les coordonnée des dqrode vues et leur id.
- les touches clavier 

Ce qui reste a faire : 
*/