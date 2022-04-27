//#include "paho.h" librairie C pour mqtt à installer https://github.com/eclipse/paho.mqtt.c


/**
 * 
 * sub et afficher la valeur lu sur le topic :      up/value/temp
 * 
 **/

#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>

void on_connect(struct mosquitto *mosq, void *obj, int rc) 
{
	printf("ID: %d\n", * (int *) obj);
	if(rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, "test/t1", 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) 
{
	printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);
}



int main() {
	
    int rc;

	mosquitto_lib_init();

	struct mosquitto *mosq = mosquitto_new("subscribe-test", true, NULL); //Create a new mosquitto client instance
	
    mosquitto_connect_callback_set(mosq, on_connect); //Set the connect callback.  This is called when the broker sends a CONNACK message in response to a connection.
	mosquitto_message_callback_set(mosq, on_message); //Set the message callback.  This is called when a message is received from the broker.
	
    // on vérifie qu'on se connecte bien au broker
	rc = mosquitto_connect(mosq, "localhost", 1883, 60); //Connect to an MQTT broker.
	if(rc) {
		printf("Could not connect to Broker with return code %d\n", rc);
		return -1;
	}

	mosquitto_loop_start(mosq); // begin of a new thread 
	printf("Press Enter to quit...\n");
	getchar();
	mosquitto_loop_stop(mosq, true); // stop of the thread

	mosquitto_disconnect(mosq); // Disconnect from the broker.
	mosquitto_destroy(mosq); // Use to free memory associated with a mosquitto client instance.
	mosquitto_lib_cleanup(); //Call to free resources associated with the library.

	return 0;
}



