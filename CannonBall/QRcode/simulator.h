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
	QRCode(float x, float y, float id){ point = new cv::Point2f(x, y); this->id = id; }
	cv::Point2f* point;
	int id;
	char str[20]; 
	char* toString() { sprintf(str, "x: %f, y: %f, id: %f\n", point->x, point->y, id); return str; }
};

class simulator
{
	
public:
	//mqtt_receiver *mqtt;
	//std::vector<QRCode>* TheMarkers;;
	mqtt_receiver *receiver;
	mqtt_sender *sender;
	int port = 1883;
	char* mqtt_host = "localhost"; // mmammar.ddns.net";//"192.168.43.105";

	//automata
	AI* automata;

	//Current state
	int steering = 90;
	int throttle = 91;

	simulator(); // std::vector<QRCode>* marker);
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