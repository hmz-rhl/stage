/**
 * 
 * @authors Hicham GHANEM et Raphaël GANDUS
 * 
 * @date 27 april 2022
 * 
 * 
 **/
 

#include "mqtt.h"


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

    int test = mosquitto_lib_init();
    if(test == MOSQ_ERR_SUCCESS){
        printf("parametre de mosquitto_loop_start valide \n");
    } 
    else if(test == MOSQ_ERR_UNKNOWN){
        printf("parametre de mosquitto_loop_start invalide \n");
    } 
    mosq = mosquitto_new("connect", true, NULL);
    if(mosq == NULL)
    {
        printf("mosquitto_new: Erreur d'instanciation d'un client mqtt \n");
    }

	rc = mosquitto_connect(mosq, "localhost", 1883, 10);
	if(rc == MOSQ_ERR_SUCCESS)
    {
        // printf("Le client n'a pas pu se connecter au broker. Message d'erreur: %d\n", rc);
        printf("We are now connected to the broker!\n");
    }
    else if(rc == MOSQ_ERR_INVAL)
    {
        printf("mosquitto_connect: parametres d'entree invalides");
    }
    
    mosquitto_destroy(mosq);
    return mosq;
	
}

/**
 ** 
 * @brief   test des topics
 *
 * @param   topic   le topic à tester
 *
 * @return 1 si le topic est dans la liste, 0 sinon
 *
 **/

/*int test_topic(char *topic)
{
    for(int cpt = 0; cpt < 25; cpt++)
    {
        if(strcmp(topic, tab_topics[cpt])==0) //voir "topic.h" pour la liste des topics, stockés dans le tableau tab_topics
        {
            return 1;
        }
        printf("pas %s\n", tab_topics[cpt]);
    }
    return 0;
}*/


//--------------------OK----------------------------------------
/**
 ** 
 * @brief   publication d'un message vers un topic, le topic doit correspondre à ceux créés par Gilles
 *
 * @param   topic   choix du topic mqtt
 * @param   message message à publier
 * @param   mosq    instance de connexion au broker
 *
 **/
void mqtt_publish(char *topic, char *message, struct mosquitto* mosq)
{
    /*while(!test_topic(topic))
    {
        printf("Erreur dans le choix du topic, veuillez choisir un topic satisfaisant :\n");
        scanf("%s", topic);
        getchar();
    }*/

    int debug = mosquitto_publish(mosq, NULL, topic, strlen(message), message, 0, false);
    if(debug == MOSQ_ERR_SUCCESS)
    {
        printf("Success");
        printf("topic :%s\n", topic);
    }
    else if(debug == MOSQ_ERR_INVAL)
    {
        printf("The input parameters were invalid.");
    }
    else if(debug == MOSQ_ERR_NOMEM)
    {
        printf("An out of memory condition occurred.");
    }
    else if(debug == MOSQ_ERR_NO_CONN)
    {
        printf("the client isn’t connected to a broker.");
    }
    else if(debug == MOSQ_ERR_PROTOCOL)
    {
        printf("there is a protocol error communicating with the broker.");
    }
    else if(debug == MOSQ_ERR_PAYLOAD_SIZE)
    {
        printf("payloadlen is too large.");
    }
    else if(debug == MOSQ_ERR_MALFORMED_UTF8)
    {
        printf("the topic is not valid UTF-8");
    }
    else if(debug == MOSQ_ERR_QOS_NOT_SUPPORTED)
    {
        printf("the QoS is greater than that supported by the broker.");
    }
    else if(debug == MOSQ_ERR_OVERSIZE_PACKET)
    {
        printf("the resulting packet would be larger than supported by the broker.");
    }
    
}



/**
 ** 
 * @brief   Affiche le topic
 *
 * @param   mosq   une instance mosquitto valide
 * @param   obj   the user data provided in mosquitto_new
 * @param   msg   le message recu
 * 
 *
 **/
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) 
{
	printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);
}



/**
 ** 
 * @brief   abonnement à un topic, le topic doit correspondre à ceux créés par Gilles
 *
 * @param   topic   choix du topic mqtt
 * @param   mosq    instance de connexion au broker
 * @param   traitement  fonction de traitement pour interagir avec les données lues sur le topic
 * 
 *
 **/
void mqtt_subscribe(char *topic, void (*traitement)(struct mosquitto *, void* , const struct mosquitto_message*), struct mosquitto* mosq)
{
    // while(!test_topic(topic))
    // {
    //     printf("Erreur dans le choix du topic, veuillez choisir un topic satisfaisant:\n");
    //     scanf("%s", topic);
    //     getchar();
    // }

    int debug = mosquitto_subscribe(mosq, NULL, topic, 0);
    if(debug == MOSQ_ERR_SUCCESS)
    {
        printf("Success");
    }
    else if(debug == MOSQ_ERR_INVAL)
    {
        printf("The input parameters were invalid.");
    }
    else if(debug == MOSQ_ERR_NOMEM)
    {
        printf("An out of memory condition occurred.");
    }
    else if(debug == MOSQ_ERR_NO_CONN)
    {
        printf("the client isn’t connected to a broker.");
    }
    else if(debug == MOSQ_ERR_MALFORMED_UTF8)
    {
        printf("the topic is not valid UTF-8");
   
    }else if(debug == MOSQ_ERR_OVERSIZE_PACKET)
    {
        printf("the resulting packet would be larger than supported by the broker.");
    }
    mosquitto_message_callback_set(mosq, traitement);
    // begin of a new thread 
    int status = mosquitto_loop_start(mosq);
    if(status == MOSQ_ERR_SUCCESS){
        printf("parametre de mosquitto_loop_start valide \n");
        printf("topic :%s\n", topic);
    } 
    else if(status == MOSQ_ERR_INVAL){
        printf("parametre de mosquitto_loop_start invalide \n");
    } 
    else if(status == MOSQ_ERR_NOT_SUPPORTED){
        printf("parametre de mosquitto_loop_start invalides \n");
    } 
	// mosquitto_loop_stop(mosq, true); // stop of the thread 
}


/**
 *
 * @brief   permet de se déconnecter du broker et de détruire l'instance mqtt
 *
 * @param   mosq  instance de connexion au brpker
 * 
 **/
void mqtt_free(struct mosquitto* mosq)
{
    if(mosq == NULL)
    {
        perror("argument NULL donc rien a free dans mqtt_free");
        return;
    }
    int status = mosquitto_loop_stop(mosq, true);
     if(status == MOSQ_ERR_SUCCESS){
        printf("parametre de mosquitto_loop_stop valide \n");
    } 
    else if(status == MOSQ_ERR_INVAL){
        printf("parametre de mosquitto_loop_stop invalide \n");
    } 
    else if(status == MOSQ_ERR_NOT_SUPPORTED){
        printf("parametre de mosquitto_loop_stop invalide \n");
    } 
    int rc = mosquitto_disconnect(mosq);
    if(rc == MOSQ_ERR_SUCCESS)
    {
        // printf("Le client n'a pas pu se connecter au broker. Message d'erreur: %d\n", rc);
        printf("On est maintenant déconnecté du broker.!\n");
    }
    else if(rc == MOSQ_ERR_INVAL)
    {
        printf("mosquitto_disconnect: parametres d'entree invalides");
    }
    else if (rc == MOSQ_ERR_NO_CONN	)
    {
         printf("the client isn’t connected to a broker.");
    }
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}

