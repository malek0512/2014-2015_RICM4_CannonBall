#include <stdio.h>
#include <tchar.h>
#include "Serial.h"	// Library described above
#include <string>
#include <iostream>
#include "test_arduino.h"
boolean DEBUG = true;

// application reads from the specified serial port and reports the collected data
int TestArduino2(int argc, _TCHAR* argv[])
{
	printf("Welcome to the serial test app!\n\n");

	Serial* SP = new Serial("\\\\.\\COM3");    // adjust as needed

	if (SP->IsConnected())
		printf("We're connected");

	int readResult;
	int cpt = 0;
	const int taille = 30;
	char tmp[1];


	//Vide le buffer en lecture
	while ((readResult = SP->ReadData(tmp, 1)) != -1);

	while (SP->IsConnected())
	{
		char buffer[2] = { 86, 88 };
		if (cpt == 10) {
			buffer[0] = 100;
			buffer[1] = 120;
			cpt = 0;
		}
		printf("Bytes writing buffer : %d, %d\n", buffer[0], buffer[1]);
		boolean ok = SP->WriteData(buffer, 2);
		
		buffer[0] = buffer[1] = 0;
		printf("Test BUFFFFFFER %d, %d\n", buffer[0], buffer[1]);

		char buffer2[2] = { 0, 0 };

		//Attend l'ecriture de tout le buffer
		while ((readResult = SP->ReadData(buffer2, 2)) == -1);
		if (readResult != 2) //A adapter selon le besoin en lecture
			printf("Attention !! Arduino -> ReadData n'a lu qu'une partie de ce qui était attendu");

		printf("Bytes read: buffer2 %d, %d\n", buffer2[0], buffer2[1]);

		Sleep(500);
		cpt++;
	}
	return 0;
}


void sendCommand2(Serial* arduin, int steering, int throttle) {
	const int nbByte = 2;

	//Initialazing the buffer
	char bufferW[nbByte] = { (char)(steering), (char)(throttle) };
	if (!(arduin)->WriteData(bufferW, nbByte)) {
		std::cout << "Serial write fail !" << std::endl;
	}

	if (DEBUG) printf("Bytes Written : %d, %d\n", bufferW[0], bufferW[1]);

	//We verify if the bytes written are the ones we send
	char bufferR[nbByte] = { 0, 0 };
	int readResult = 0;
	while ((readResult = (arduin)->ReadData(bufferR, nbByte)) == -1)
		printf("WAZA Bytes Read : %d, %d\n", bufferR[0], bufferR[1]);
	
	if (DEBUG) printf("Bytes Read : %d, %d\n", bufferR[0], bufferR[1]);
	if (readResult != nbByte)
		printf("Attention !! Arduino -> ReadData n'a lu qu'une partie de ce qui était attendu \n");

	if (!strcmp(bufferW, bufferR))
		printf("ERROR ! (send_command) The bytes we sent doesn't match with the on we have written ! \n");

}

// application reads from the specified serial port and reports the collected data
int TestArduino(int argc, _TCHAR* argv[])
{
	printf("Welcome to the serial test app!\n\n");

	Serial* SP = new Serial("\\\\.\\COM3");    // adjust as needed

	if (SP->IsConnected())
		printf("We're connected");

	int readResult;
	int cpt = 0;
	const int taille = 30;
	char tmp[1];

	Serial* arduin = SP;
	int steering = 86, throttle = 88;
	

	//Vide le buffer en lecture
	while ((readResult = SP->ReadData(tmp, 1)) != -1);

	while (SP->IsConnected())
	{
		
		sendCommand2(SP, 86, 88);

		Sleep(500);
		cpt++;
	}
	return 0;
}

