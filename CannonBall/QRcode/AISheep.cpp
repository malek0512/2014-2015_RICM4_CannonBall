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

AISheep::AISheep() {
	idle = 0;
}

AISheep::~AISheep() {
	// TODO Auto-generated destructor stub
}

void AISheep::getCommand(vector<aruco::Marker>* TheMarkers, int* steering, int* throttle, int width) {
	if(idle == 0)
	{//si aucune action a faire
		//cherche un qrCode
		unsigned float minDist = -1;
		for (std::vector<Marker>::iterator it = (*TheMarkers).begin(); it != (*TheMarkers).end(); it++) {
			if(minDist > it->Tvec.ptr<float>(0)[2])
			{
				minDist = it->Tvec.ptr<float>(0)[2];
			}

		}
		//si aucun qrCode, continuer a chercher
		//sinon realiser action qrCode
	}
	else
	{//sinon continuer l'action
		switch(idle){
		case 1:break;
		default : break;
		}
	}
}

