/*
 * AISheep.h
 *
 *  Created on: 23 févr. 2015
 *      Author: alex
 */

#include <map>
#include "AI.h"

class AISheep :
	public AI
{
public:
	AISheep(int argc, char *argv[]);
	virtual ~AISheep();
	virtual int convertIDtoAction(int id);
	virtual void getCommand(
				vector<aruco::Marker>* TheMarkers,
				int* steering,
				int* throttle,
				int width);

private :
		int idle;
		double deltaTime;
		std::vector<std::pair<int,int>> tableIDAction;
};

