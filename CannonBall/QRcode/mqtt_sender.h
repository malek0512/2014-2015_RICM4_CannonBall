#ifndef MQTT_SENDER_H
#define MQTT_SENDER_H

#include <mosquittopp.h>
#include <stdint.h>
#include <string>

#define TOPIC_STEER				"metrics/steering"
#define TOPIC_THROT				"metrics/throttle"
#define TOPIC_LAPS				"metrics/laps"
#define TOPIC_AVG				"metrics/avg"
#define TOPIC_MODE				"metrics/mode"
#define TOPIC_ACCELEROMETER		"metrics/accelerometer"
#define TOPIC_NB_MARKERS		"metrics/nb_markers"
#define TOPIC_CLOSEST			"metrics/closest"

#define TOPIC_COMMANDS_CAMERA			"camera/commands"
#define TOPIC_COMMANDS_STOP_THE_CAR		"camera/commands"
#define TOPIC_CAMERA_IMAGE				"camera/images"
#define MOSQUITTO_HIGH_PRIORITY			2
#define MOSQUITTO_MIDLE_PRIORITY		1
#define MOSQUITTO_LOW_PRIORITY			0

#if defined(__linux__)
#define MOSQPP mosquittopp
#elif defined(_WIN32) || defined(__APPLE__)
#define MOSQPP mosqpp
#endif

/**
 * Class which allow to send message throw MQTT protocol
 * @author Thibaut Coutelou, Benjamin Mugnier, Guillaume Perrin
 * Modified by Malek MAMMAR
 */
class mqtt_sender : public mosqpp::mosquittopp
{
    public:
        /**
         * Constructor, create a connection
         * @param id the sender id
         * @param host address of the MQTT server
         * @param port port used by the MQTT server
         */
        mqtt_sender(const char *id, const char *host, int port);
        ~mqtt_sender();

        /**
         * Send message to the MQTT server, if connected
         * @param topic the topic in which the message will be send
         * @param msg message to send
         */
        void publish_to_topic(char *topic, char *msg);
		void subscribe_to_topic(char* topic);
		void set_callback(void callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message));
		void subscribe_init();
		mosquitto *mosq;
   private:
        bool connected;

		int id;
		const char* host;
		int port;

};

#endif
