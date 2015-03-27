#include "car.h"
#include "test_arduino.h"
//#include "simulator.h"


int main(int argc, char *argv[]) {
	/* 
	if (!strcmp(argv[5], "Simulator")) {
		simulator(argv);
	}
	else 
	*/
	
	(new car())->main(argc, argv);
	//(new simulator())->main(argc, argv);
	
	//TestArduino(argc, argv);
	return 0;
}
