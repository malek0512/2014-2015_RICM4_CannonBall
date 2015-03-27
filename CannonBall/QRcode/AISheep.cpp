/*
 * AISheep.cpp
 *
 *  Created on: 23 févr. 2015
 *      Author: alex
 */

#include "AISheep.h"
#include <iostream>
#include <fstream>
#include <queue>
#include "Windows.h"
#include <iostream>
#include <Windows.h>

using namespace aruco;


AISheep::AISheep(int argc, char *argv[]) {
	if (argc < 1) {
		std::cerr << "AI  Sheep usage : <actions.xml>" << std::endl;
		std::cerr << "<actions.xml> : xml file which give qrcode ID for each action wanted" << std::endl;
		Sleep(10000);
		exit(-1);
	}
	//initialisation des variables
	idle = 0;
	deltaTime = -1;

	tableIDAction.push_back(std::make_pair(1, 20259)); //stop
	tableIDAction.push_back(std::make_pair(2, 11405)); //left
	tableIDAction.push_back(std::make_pair(3, 42307)); //right


	/*
	//lecture du fichier xml
	TiXmlDocument doc("conf.xml");
	if (!doc.LoadFile()){ // on verifie le chargement du fichier xml
		cerr << "erreur lors du chargement" << endl;
		cerr << "error #" << doc.ErrorId() << " : " << doc.ErrorDesc() << endl;
		return 1;
	}

	// on se place dans la premiere ligne de config
	TiXmlElement *elem = doc.FirstChildElement()->FirstChildElement();

	if (elem->Attribute("name") == "nbActions") {
		nbActions = elem->Attribute("value");
	}
	// création du tableau d'actions

	// bla bla bla


	// on se place maintenant dans la partie actions
	TiXmlElement *elem = doc.FirstChildElement();
	elem = elem->NextSiblingElement();
	elem = elem->FirstChildElement();
	while (elem) {
		String action = elem->Attribute("name");
		String qrCode = elem->Attribute("value");
		elem = elem->NextSiblingElement();
	}
	*/
}

AISheep::~AISheep() {
	// TODO Auto-generated destructor stub
}

int AISheep::convertIDtoAction(int id)
{
	int res = 1;
	
	return res;
}

void AISheep::getCommand(vector<aruco::Marker>* TheMarkers, int* steering, int* throttle, int width) {
	
	int timeOutAction = 10000;
	//initialisation du vehicule
	if (deltaTime == -1)
	{
		*throttle = 89;
		*steering = 90;
		deltaTime = 0;
	}
	if(idle == 0)
	{//si aucune action a faire
		//cherche un qrCode
		float distMinRequire = 3;
		float minDist = distMinRequire;

		//recherche des qrCode
		int id;
		{
			for (std::vector<Marker>::iterator it = (*TheMarkers).begin(); it != (*TheMarkers).end(); it++) {
				if (minDist > it->Tvec.ptr<float>(0)[2])
				{
					minDist = it->Tvec.ptr<float>(0)[2];
					id = it->id;
				}

			}
		}

		//si la distance du plus proche n'est pass asser bonne
		if (minDist > distMinRequire) {
			deltaTime += (double) GetTickCount();
			if (deltaTime > timeOutAction)
				idle = 1;
			return;
		}

		else
		{
			//raz du timer
			deltaTime = 0;
			//recherche de l'action a faire
			{
				idle = 1;
				std::vector<std::pair<int, int>>::iterator it = tableIDAction.begin();
				while (it != tableIDAction.end() && it->second != id)
				{
					it++;
				}
				idle = it->first;
			}
		}

		//si aucun qrCode, continuer a chercher
		//sinon realiser action qrCode
	}
	deltaTime += (double)GetTickCount();

	switch (idle){
		case 1: //stop
			*throttle = 91;
			*steering = 90;
			break;
		case 2: //turn left
			*steering = 70;
			if (deltaTime > timeOutAction)
			{
				idle = 0;
				deltaTime = 0;
				*steering = 90;
			}
			break;
		case 3: //turn right
			*steering = 110;
			if (deltaTime > timeOutAction)
			{
				idle = 0;
				deltaTime = 0;
				*steering = 90;
			}
			break;
		default: break;
	}
	

}


