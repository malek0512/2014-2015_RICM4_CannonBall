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
	AISheep();
	virtual ~AISheep();
	virtual void getCommand(
				vector<aruco::Marker>* TheMarkers,
				int* steering,
				int* throttle,
				int width);

private :
		int idle;
};

