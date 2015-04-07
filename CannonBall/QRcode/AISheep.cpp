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
#include <stdio.h>

using namespace aruco;
	
AISheep::AISheep(int argc, char *argv[]) {
	if (argc < 1) {
		std::cerr << "AI  Sheep usage : <actions.act>" << std::endl;
		std::cerr << "<actions.act> : file which give qrcode ID for each action wanted" << std::endl;
		Sleep(10000);
		exit(-1);
	}
	//initialisation des variables
	idle = INIT;
	deltaTime = -1;
	idCurrentAction = -1;
	//parse du fichier
	std::list<std::string> tmp = AISheep::lexer(argv[0]);

	parse(tmp);
	std::cout << "yo, apachai dayo";

}

AISheep::~AISheep() {
	// TODO Auto-generated destructor stub
}

std::list<std::string> AISheep::lexer(std::string filepath)
{
	std::ifstream ifs;
	ifs.open(filepath);
	
	char tokenlist[] = { '[', ']', '=', '-', '/' };
	bool istoken;
	std::list<std::string> tokenExtracted;
	char charCourant;

	while (!ifs.eof())
	{
		charCourant = ifs.get();

		//met a jour istoken pour next
		{
			int i = 0;
			while (i < std::strlen(tokenlist) && tokenlist[i] != charCourant)
			{
				i++;
			}
			if (tokenlist[i] == charCourant)
			{
				string nextToken;
				nextToken += charCourant;
				tokenExtracted.push_back(nextToken);
				continue;
			}
		}
		string nextToken;
		//nextToken += charCourant;
		//si on a pas un token, on recupere autant de charactere alpha num que possible
		if (isalnum(charCourant))
		{
				while (!ifs.eof() && isalnum(charCourant))
				{
					nextToken += charCourant;
					charCourant = ifs.get();
					
				}
			//on push le token lu
			tokenExtracted.push_back(nextToken);
		}
	}
	return tokenExtracted;
}


void AISheep::parse(std::list<std::string> tokens)
{
	int _DValue_throttle = 91; //valeur par defaut de la vitesse
	int _DValue_steering = 90; //valeur par defaut de l'orientation
	int _DValue_time = 1000; //valeur par defaut du temps passé dans une action
	std::list<std::string>::iterator i = tokens.begin();
	while (i !=  tokens.end()) {
		//lecture de l'action
		int id;
		//action act;
		{
			// lecture de token
			if (*i != "[")
				throw 0;
			
			//incrementation
			i++;
			if(i == tokens.end())
				throw 0;

			//lecture de l'id
			try{
			id = std::stoi(*i);
			}
			catch( ... ){throw 0;}
			
			//incrementation
			i++;
			if(i == tokens.end())
				throw 0;
						
			// lecture de token
			if (*i != "]")
				throw 0;
			//incrementation
			i++;
			if(i == tokens.end())
				throw 0;

		}

		//lecture des paramètres associé a l'id
		struct param param;
		{
			//valeur par default
			param.throttle = _DValue_throttle;
			param.steering = _DValue_steering;
			param.time = _DValue_time;

			//lecture de chaque parametre configurer
			while (i != tokens.end() && (*i == "-" || *i == "/"))
			{
				//incrementation
				i++;
				if(i == tokens.end())
					throw 0;
				
				if(*i == "/")
				{//nouvelle sequence d'action
					tableIDAction.push_back(std::make_pair(id, param));
					
					//on remet les valeurs par defaut
					param.throttle = _DValue_throttle;
					param.steering = _DValue_steering;
					param.time = _DValue_time;

					//incrementation
					i++;
					if(i == tokens.end())
						throw 0;
					continue;
				}
						
				//lecture du nom du parametre
				std::string name_param = *i;
				
				//incrementation
				i++;
				if(i == tokens.end())
					throw 0;

				// lecture de token 
				if (*i != "=")
					throw 0;
				
				//incrementation
				i++;
				if(i == tokens.end())
					throw 0;

				//lecture de la valeur
				int valeur_param;
				try{
					valeur_param = std::stoi(*i);
				}
				catch( ... ){throw 0;}
				
				//on enregistre les informations dans param
				if (name_param.compare("speed") == 0)
				{
					switch (valeur_param)
					{
					case 0:
						param.throttle = 91; break;

					case 1:
						param.throttle = 90; break;

					case 2:
						param.throttle = 89; break;

					case 3:
						param.throttle = 88; break;

					case 4:
						param.throttle = 86; break;

					default:
						param.throttle = 86; break;
					}
				}
				else if (name_param.compare("orientation") == 0)
					param.steering = valeur_param;
				else if (name_param.compare("time") == 0)
					param.time = valeur_param;
				else
					throw 0;
				
				//incrementation
				i++;
			}

		}

		//on enregistre les resultats dans le tableau id-action
		tableIDAction.push_back(std::make_pair(id, param));
	}
}

void AISheep::getCommand(vector<aruco::Marker>* TheMarkers, int* steering, int* throttle, int width) {
	
	//initialisation du vehicule
	if (deltaTime == -1)
	{
		*throttle = 91;
		*steering = 90;
		deltaTime = 0;
	}
			
	else
	{
		//cherche un qrCode
		float distMinRequire = -1; //distance minimal necessaire pour accepter de faire le QrCode vue
		float minDist = distMinRequire;

		//recherche des qrCode
		int id = -1;
		{
			for (std::vector<Marker>::iterator it = (*TheMarkers).begin(); it != (*TheMarkers).end(); ++it) {
				if (minDist == -1)
					minDist = it->Tvec.ptr<float>(0)[2];
				if (minDist >= it->Tvec.ptr<float>(0)[2])
				{
					minDist = it->Tvec.ptr<float>(0)[2];
					id = it->id;
				}

			}
		}
		
		printf("idCurrent Action : %d       ###     id : %d \n", idCurrentAction, id);
		
		//no action ever seen
		if ((*TheMarkers).size() == 0 && idCurrentAction == -1)
			return;

		//Une nouvelle action est a traité
		if( (idCurrentAction == -1 && id != -1) 
			||(idCurrentAction != -1 && id != -1 && idCurrentAction != id))
		{
			deltaTime = 0;
			
			itCurrentAction  = tableIDAction.begin();
			while (itCurrentAction != tableIDAction.end() && itCurrentAction ->first != id)
			{
				itCurrentAction ++;
			}
			
			if (itCurrentAction != tableIDAction.end())
			{
				idCurrentAction = id;
				*steering = itCurrentAction->second.steering;
				*throttle = itCurrentAction->second.throttle;
				timeOutAction = itCurrentAction->second.time;
			}
			
			return;
		}
		
		//On suit le cours normal de l'action courante)
		if(idCurrentAction != -1 && id == -1)
		{
			deltaTime++;
			if (deltaTime > timeOutAction)
			{
				itCurrentAction++;
				if(itCurrentAction != tableIDAction.end() && itCurrentAction->first == idCurrentAction)
				{ //il y a une autre action associé a l'id en cours
					deltaTime = 0;
					*steering = itCurrentAction->second.steering;
					*throttle = itCurrentAction->second.throttle;
					timeOutAction = itCurrentAction->second.time;
				}
				else
				{
					*throttle = 91;
					*steering = 90;
					idCurrentAction = -1;
				}
			}			
			return;
		}
	}
		
}
