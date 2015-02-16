#include <Servo.h>

/* Constant used to communicate over the serial */
#define BAUDRATE 115200

/* Constants which define pins used by servos */
#define STEERING_SERVO_PIN (5)
#define THROTTLE_SERVO_PIN (6)
#define LED_PIN (13)

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
unsigned char isComputerReady = 0;

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

    //Boucle d'attente active afin d'inhiber l'entrée en état "Non reception de donner du pc, arret d'urgence enclenché"
    while ( isComputerReady != 255 ) {
      if ( Serial.available() >= 1 ) {
        isComputerReady = Serial.read();
      }
    }
    last_time_data_check = millis();
}

void loop() {
    time_data_check = millis();
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
    
    SteeringServo.write(steeringTarget);
    ThrottleServo.write(throttleTarget);

    //EMERGENCY STATE
    if (emergency || ((time_data_check - last_time_data_check) > 1000) ) {
        digitalWrite(13, HIGH); //for debug purposes only

        if (throttleTarget < 89) { //high speed
            for (;;) {
                ThrottleServo.write(130); //brake
                //Serial.write((char) 42); //for debug purposes only
                if ( Serial.available() >= 1 ) {
                  if ((unsigned char) Serial.read() == 255){
                    break;
                  }
                }
            }
        }
        else {
            for (;;){
                ThrottleServo.write(91); //only slow down, not to go reverse
                //Serial.write((char) 42); //for debug purposes only
                if ( Serial.available() >= 1 ) {
                  if ((unsigned char) Serial.read() == 255){
                    break;
                  }
                }
            }
        }
    }
}
