#include <Servo.h>

/* Constant used to communicate over the serial */
#define BAUDRATE 115200

/* Constants which define pins used by servos */
#define STEERING_SERVO_PIN (5)
#define THROTTLE_SERVO_PIN (6)
#define LED_PIN (13)
#define TIME 5000

/* Variable declaration */
Servo SteeringServo;
Servo ThrottleServo;
unsigned int steeringTarget = 90;
unsigned int throttleTarget = 91;

unsigned long time_rc_handler = 0;
unsigned long time_data_check = 0;
unsigned long last_time_data_check = 0;
unsigned long last_time_rc_handler = 0;
unsigned long period = 0;
boolean emergency = false;

const unsigned char readyToGo = 255;
unsigned int emergencyTime = 1000;

void rc_handler() {
    //BE CAREFUL, DO NOT PRINT STUFF HERE
    //Serial.println use interrupts too, thus resulting in a deadlock :(

    time_rc_handler = micros(); //CAREFUL, micro will overflow after 70min (returning 0)
    period = time_rc_handler - last_time_rc_handler;
    if (period < 1000) {
        //We've got an emergency, stop everything
        emergency = true;
    }
    last_time_rc_handler = time_rc_handler;
}

//Lis n bytes dans le tableau préalablement alloué et de taille n
void readSerial(char* bytes, unsigned int n) {
  for(int i=0; i<n; i++) {
    while (Serial.available() == 0);
    bytes[i] = Serial.read();
  }
}
//Ecrit n bytes dans le tableau préalablement initialisé et de taille n
void writeSerial(char* bytes, unsigned int n) {
  for(int i=0; i<n; i++) {
    Serial.write(bytes[i]);
    Serial.flush();
  }
}

void setup() {
    /* Output */
    pinMode(STEERING_SERVO_PIN, OUTPUT);
    pinMode(THROTTLE_SERVO_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    digitalWrite(LED_PIN, LOW);
    SteeringServo.attach(STEERING_SERVO_PIN);
    ThrottleServo.attach(THROTTLE_SERVO_PIN);

    attachInterrupt(1, rc_handler, CHANGE); //1 is pin 3 (0 pin 2)

    Serial.begin(BAUDRATE);

    //Recuperation de la durée maximale autorisée d'absence de communications entre Tablette-Arduino
//    readSerial((char*) emergencyTime, 4);
//    writeSerial((char*) emergencyTime, 4); //Confirmation de la reception
    
    //Boucle d'attente active afin d'inhiber l'entrée en état "Non reception de donner du pc, arret d'urgence enclenché"
    //Dans le cas d'un pc lent, la partie initialisation des autre composants peut retarder la communication avec l'arduino
    unsigned char isComputerReady = 0;
    while ( isComputerReady != readyToGo ) {
      if ( Serial.available() >= 1 ) {
        isComputerReady = Serial.read();
      }
    }
    last_time_data_check = millis();
}

void loop() {
    time_data_check = millis();
    
    //Communication serial
    if (Serial.available() >= 1) {
        steeringTarget = Serial.read();   
        Serial.write((char)steeringTarget);
        Serial.flush();
    }

    if (Serial.available() >= 1) {    
        throttleTarget = Serial.read();
        Serial.write((char)throttleTarget);
        Serial.flush();
        last_time_data_check = time_data_check;
    }
    
    //Communication aux pins
    SteeringServo.write(steeringTarget);
    ThrottleServo.write(throttleTarget);

    //EMERGENCY STATE
    if (emergency || ((time_data_check - last_time_data_check) > TIME) ) {
        digitalWrite(13, HIGH); //for debug purposes only

        if (throttleTarget < 89) { //high speed
            for (;;) {
                ThrottleServo.write(130); //brake
                if ( Serial.available() >= 1 ) {
                  if ((unsigned char) Serial.read() == readyToGo){ //Le programme est réinitialiser --> sortie de l'etat d'urgence
                    break;
                  }
                }
            }
        }
        else {
            for (;;){
                ThrottleServo.write(91); //only slow down, not to go reverse
                if ( Serial.available() >= 1 ) {
                  if ((unsigned char) Serial.read() == readyToGo){ //Le programme est réinitialiser --> sortie de l'etat d'urgence
                    break;
                  }
                }
            }
        }
    }
}
