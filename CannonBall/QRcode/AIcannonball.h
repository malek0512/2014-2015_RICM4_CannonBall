#pragma once
#include "AI.h"
#include <map>
#include <stdbool.h>

/**
 * liste chainé representant les portes pour le parcour
 * Attention : il sagit d'un circuit, donc le suivant du dernier element de la liste pointe sur le premier.
 */
struct door {
	int left; //Verifier: si la valeur represente la valeur de l'id d'un marker, se serai alors le marker de gauche et de droite qui definise une porte dans laquel il faut passer pour continuer le circuit
	int right;
	struct door* next;
};

/**
 * This artificial intelligence alows the car to follow a circuit.
 * The doors of the circuit are markers. The markers order is given in a file :
 * left1 right1 left2 right2...
 */
class AICannonball :
	public AI
{
public:
	AICannonball(int argc, char *argv[]);
	~AICannonball();

	/**
	* Action realized by the AI
	* @param TheMarkers markers found
	* @param steering the new value of the steering
	* @param throttle the new value of the throttle
	* @param width capture width in pixel
	*/
	virtual void getCommand(
			vector<aruco::Marker>* TheMarkers,
			int* steering,
			int* throttle,
			int width);

private:
	struct element {
		aruco::Marker marker;
		int lastTimeSeen; //derniere frame ou a était vue le marker
		bool valid; // vrai si le marker est présent sur la fram actuel
	};
	int frame;
	int idle;
	map<int, struct element> elements;
	//pointeur sur la porte actuel
	struct door* currentDoor;
};


