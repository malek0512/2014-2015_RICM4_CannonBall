#pragma once
#include "mqtt_receiver.h"
#include "mosquitto.h"
#include "AI.h"
#include <map>
#include "pugixml.hpp"

#define SIMULATOR_STEERING		"simulator/steering" //entier
#define SIMULATOR_THROTTLE		"simulator/throttle" //entier
#define SIMULATOR_CAR			"simulator/car"		//x:y	
#define SIMULATOR_ADD			"simulator/addqrcode" //k:value:x:y k=position dans la liste, value (id 20259) 
#define SIMULATOR_DEL			"simulator/delqrcode" //k:value:x:y
#define SIMULATOR_MOVE			"simulator/movqrcode" //k:value:x:y

#define HOST "192.168.43.105"
#define PORT 1883

//int port = 1883;
//char* mqtt_host = "localhost"; // mmammar.ddns.net";//"192.168.43.105";

/*FORMAT COORDONNEES <ID:x,y>*/
class QRCode {
public :
	QRCode(float x, float y, float id){ point = new cv::Point2f(x, y); this->id = id; }
	cv::Point2f* point;
	int id;
	char str[20]; 
	char* toString() { sprintf(str, "x: %f, y: %f, id: %f\n", point->x, point->y, id); return str; }
	double distance(cv::Point2f* q) {
		return cv::norm(*point - *(q));
	}
	
};

class simulator
{
	
public:
	//mqtt_receiver *mqtt;
	//std::vector<QRCode>* TheMarkers;;

	//automata
	AI* automata;

	//Current state
	int steering = 90;
	int throttle = 91;

	simulator(); // std::vector<QRCode>* marker);
	~simulator();
	void main(pugi::xml_document* doc);
	void simulator::sendSimulator(int steering, int throttle);

private:
	void initMQTT();
	mqtt_receiver *receiver;
	mqtt_sender *sender;
};



/*
Ce qui est imortant : 
- gerer la cllision et sortir d'ecran c'est lunivers qui se deplace soit thore
- envoyer les coordonnée des dqrode vues et leur id.
- les touches clavier 

Ce qui reste a faire : 
*/