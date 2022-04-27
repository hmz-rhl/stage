#include "mqtt.h"
#include "topic.h"


/**
 ** 
 * @brief   configure l'interface i2c, et lui fait connaitre l'adresse de l'expander
 * 
 * @param   exp pointeur sur variable structuré de l'expander a labeliser
 * 
 * @return  
 **/
struct mosquitto* init_mqtt()
{
    int rc;
	struct mosquitto * mosq;

    mosquitto_lib_init();
    mosq = mosquitto_new("connect", true, NULL);

	rc = mosquitto_connect(mosq, "localhost", 1883, 60);
	if(rc != 0){
		printf("Client could not connect to broker! Error Code: %d\n", rc);
		mosquitto_destroy(mosq);
		return -1;
	}
	printf("We are now connected to the broker!\n");

    return mosq;
}

void publish(char *topic, char *message)
{
    while(!test_topic(topic))
    {
        printf("Erreur dans le choix du topic, veuillez choisir un topic satisfaisant");
    }
    mosquitto_publish(init_mqtt(), NULL, "topic", 6, "message", 0, false);
}