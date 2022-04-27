#include "mqtt.h"
#include "topic.h"


/**
 ** 
 * @brief   initialisation : création d'une instance de connexion au broker, validation de la connexion
 *
 * @return  struct mosquitto* qui sera nécessaire pour l'utilisation de la fonction publish
 *
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

bool test_topic(char *topic)
{
    for(int i = 0; i < 23; i++)
    {
        if(strcmp(topic, ))
    }
}



/**
 ** 
 * @brief   publication d'un message vers un topic, le topic doit correspondre à ceux créés par Gilles
 *
 * @param   topic   choix du topic mqtt
 * @param   message message à publier
 *
 **/
void publish(char *topic, char *message)
{
    while(!test_topic(topic))
    {
        printf("Erreur dans le choix du topic, veuillez choisir un topic satisfaisant");
    }
    mosquitto_publish(init_mqtt(), NULL, "topic", 6, "message", 0, false);
}

