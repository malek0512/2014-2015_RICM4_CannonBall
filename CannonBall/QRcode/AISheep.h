/*
 * AISheep.h
 *
 *  Created on: 23 févr. 2015
 *      Author: alex
 */

#include <map>
#include "AI.h"
#include <cstdio>
#include <list>

class AISheep :
	public AI
{
public:
	/**
	* Description des actions :
	* INIT : etat de recherche de QrCode
	* END : fin du programme
	*/
	enum action {INIT, END, OTHER};

	AISheep(int argc, char *argv[]);
	virtual ~AISheep();

	/**
	* Retite tout les caractères qui ne font pas parti des tokens
	* @param ifs : stream a traité
	* @return : String contenant seulement token selon la grammaire attendu
	*/
	std::list<std::string> lexer(std::string filepath);
	/**
	* Parse une liste de token et renvoie la table ID-Action a partir des tokens lu
	* @param tokens : suite de token a parser
	*/
	void parse(std::list<std::string> tokens);
	
	// overload
	virtual void getCommand(
				vector<aruco::Marker>* TheMarkers,
				int* steering,
				int* throttle,
				int width);

	private :
		//type des parametre que l'on peut mettre dans le fichier a parser
		struct param
		{
			int throttle;
			int steering;
			int time;
		};
		
		int idle;
		std::vector<std::pair<int, struct param>>::iterator itCurrentAction; // iterator de l'action courante
		int timeOutAction; //temps maximal a passer dans une action
		int idCurrentAction; //id de l'action en cours
		double deltaTime; //temps passer dans l'action
		std::vector<std::pair<int, struct param>> tableIDAction; //table de correspondance entre les action et l'ID des QrCode
};

