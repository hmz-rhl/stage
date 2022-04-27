//Hicham GHANEM et Raphaël GANDUS

#include "mqtt.h"
#include "topic.h"


/**
 ** 
 * @brief   initialisation : création d'une instance de connexion au broker, validation de la connexion
 *
 * @return  struct mosquitto* qui sera nécessaire pour l'utilisation des fonctions mqtt_publish et mqtt_subscribe
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

/**
 ** 
 * @brief   test des topics
 *
 * @param   topic   le topic à tester
 *
 * @return  true si le topic est dans la liste, faux sinon
 *
 **/
bool test_topic(char *topic)
{
    for(int cpt = 0; cpt < 23; cpt++)
    {
        if(strcmp(topic, tab_topics[cpt])!=0) //voir "topic.h" pour la liste des topics, stockés dans le tableau tab_topics
        {
            return false;
        }
    }
    return true;
}



/**
 ** 
 * @brief   publication d'un message vers un topic, le topic doit correspondre à ceux créés par Gilles
 *
 * @param   topic   choix du topic mqtt
 * @param   message message à publier
 *
 **/
void mqtt_publish(char *topic, char *message)
{
    while(!test_topic(topic))
    {
        printf("Erreur dans le choix du topic, veuillez choisir un topic satisfaisant");
    }

    mosquitto_publish(init_mqtt(), NULL, "topic", 6, "message", 0, false);
    
    mosquitto_disconnect(init_mqtt());
	mosquitto_destroy(init_mqtt());
	mosquitto_lib_cleanup();
}

/**
 ** 
 * @brief   Affiche le topic
 *
 * @param   mosq   une instance mosquitto valide
 * @param   obj   the user data provided in mosquitto_new
 * @param   msg   une instance mosquitto valide
 * 
 *
 **/
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) 
{
	printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);
}

/**
 ** 
 * @brief   subscription à un topic, le topic doit correspondre à ceux créés par Gilles
 *
 * @param   topic   choix du topic mqtt
 * 
 *
 **/
void mqtt_subscribe(char *topic)
{
    while(!test_topic(topic))
    {
        printf("Erreur dans le choix du topic, veuillez choisir un topic satisfaisant");
    }
    mosquitto_subscribe(init_mqtt(), NULL, "topic", 6, "message", 0, false);
    mosquitto_message_callback_set(init_mqtt(), on_message);

    mosquitto_disconnect(init_mqtt());
	mosquitto_destroy(init_mqtt());
	mosquitto_lib_cleanup();
}

