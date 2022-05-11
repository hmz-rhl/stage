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
        printf("%s: dans mosquitto_lib_init success \n", __func__);
    } 
    else if(test == MOSQ_ERR_UNKNOWN){
        printf("%s: dans mosquitto_lib_init parametres invalide \n", __func__);
        return NULL;
    } 
    mosq = mosquitto_new("connect", true, NULL);
    if(mosq == NULL)
    {
        printf("%s: dans mosquitto_new Error d'instanciation d'un client mqtt \n", __func__);
        return NULL;
    }

	rc = mosquitto_connect(mosq, "localhost", 1883, 10);
	if(rc == MOSQ_ERR_SUCCESS)
    {
        // printf("Le client n'a pas pu se connecter au broker. Message d'erreur: %d\n", rc);
        printf("%s: dans mosquitto_connect We are now connected to the broker!\n", __func__);
    }
    else if(rc == MOSQ_ERR_INVAL)
    {
        printf("%s: dans mosquitto_connect Error parametres d'entree invalides \n", __func__);
        return NULL;
    }
    
    //mosquitto_destroy(mosq);
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
        printf("%s: mosquitto_publish success \n", __func__);
        printf("topic :%s\n", topic);
    }
    else if(debug == MOSQ_ERR_INVAL)
    {
        printf("%s: dans mosquitto_publish Error The input parameters were invalid. \n", __func__);
        return;
    }
    else if(debug == MOSQ_ERR_NOMEM)
    {
        printf("%s: dans mosquitto_publish Error An out of memory condition occurred. \n", __func__);
        return;
    }
    else if(debug == MOSQ_ERR_NO_CONN)
    {
        printf("%s: dans mosquitto_publish Error the client isn’t connected to a broker. \n", __func__);
        return;
    }
    else if(debug == MOSQ_ERR_PROTOCOL)
    {
        printf("%s: dans mosquitto_publish there is a protocol Error communicating with the broker. \n", __func__);
        return;
    }
    else if(debug == MOSQ_ERR_PAYLOAD_SIZE)
    {
        printf("%s: dans mosquitto_publish Error payloadlen is too large. \n", __func__);
        return;
    }
    else if(debug == MOSQ_ERR_MALFORMED_UTF8)
    {
        printf("%s: dans mosquitto_publish Error the topic is not valid UTF-8 \n", __func__);
        return;
    }
    else if(debug == MOSQ_ERR_QOS_NOT_SUPPORTED)
    {
        printf("%s: dans mosquitto_publish Error the QoS is greater than that supported by the broker. \n", __func__);
        return;
    }
    else if(debug == MOSQ_ERR_OVERSIZE_PACKET)
    {
        printf("%s: dans mosquitto_publish Error the resulting packet would be larger than supported by the broker. \n", __func__);
        return;
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
        printf("%s: dans mosquitto_subscribe, abonnement Success\n", __func__);
    }
    else if(debug == MOSQ_ERR_INVAL)
    {
        printf("%s: dans mosquitto_subscribe, Error The input parameters were invalid.\n", __func__);
        return;

    }
    else if(debug == MOSQ_ERR_NOMEM)
    {
        printf("%s: dans mosquitto_subscribe, Error An out of memory condition occurred.\n", __func__);
        return;

    }
    else if(debug == MOSQ_ERR_NO_CONN)
    {
        printf("%s: dans mosquitto_subscribe, Error the client isn’t connected to a broker.\n", __func__);
        return;

    }
    else if(debug == MOSQ_ERR_MALFORMED_UTF8)
    {
        printf("%s: dans mosquitto_subscribe, Error the topic is not valid UTF-8\n", __func__);
        return;

   
    }else if(debug == MOSQ_ERR_OVERSIZE_PACKET)
    {
        printf("%s: dans mosquitto_subscribe, Error the resulting packet would be larger than supported by the broker.\n", __func__);
        return;

    }
    mosquitto_message_callback_set(mosq, traitement);
    // begin of a new thread 
    int status = mosquitto_loop_start(mosq);
    if(status == MOSQ_ERR_SUCCESS){
        printf("%s: dans mosquitto_loop_start success\n", __func__);
        printf("topic :%s\n", topic);
    } 
    else if(status == MOSQ_ERR_INVAL){
        printf("%s: dans mosquitto_loop_start Error parametre invalide \n", __func__);
        return;

    } 
    else if(status == MOSQ_ERR_NOT_SUPPORTED){
        printf("%s: dans mosquitto_loop_start Error parametre invalides \n", __func__);
        return;

    } 
	// mosquitto_loop_stop(mosq, true); // stop of the thread 
}

/**
 ** 
 * @brief   abonnement à plusieurs topic avec une même instance, le topic doit correspondre à ceux créés par Gilles
 *
 * @param   topics   tableaux de topics mqtt
 * @param   len nombre de topics auxquels s'abonner
 * @param   traitement  fonction de traitement pour interagir avec les données lues sur le topic
 * @param   mosq    instance de connexion au broker
 * 
 *
 **/

void mqtt_subscribe_multiple(char *const *const topics, int len, void (*on_message)(struct mosquitto *, void* , const struct mosquitto_message*), struct mosquitto* mosq)
{
    int debug = mosquitto_subscribe_multiple(mosq, NULL, len, topics, 0, NULL, NULL);
    if(debug == MOSQ_ERR_SUCCESS)
    {
        printf("%s: dans mosquitto_subscribe, abonnement Success\n", __func__);
    }
    else if(debug == MOSQ_ERR_INVAL)
    {
        printf("%s: dans mosquitto_subscribe, Error The input parameters were invalid.\n", __func__);
        return;

    }
    else if(debug == MOSQ_ERR_NOMEM)
    {
        printf("%s: dans mosquitto_subscribe, Error An out of memory condition occurred.\n", __func__);
        return;

    }
    else if(debug == MOSQ_ERR_NO_CONN)
    {
        printf("%s: dans mosquitto_subscribe, Error the client isn’t connected to a broker.\n", __func__);
        return;

    }
    else if(debug == MOSQ_ERR_MALFORMED_UTF8)
    {
        printf("%s: dans mosquitto_subscribe, Error the topic is not valid UTF-8\n", __func__);
        return;

   
    }else if(debug == MOSQ_ERR_OVERSIZE_PACKET)
    {
        printf("%s: dans mosquitto_subscribe, Error the resulting packet would be larger than supported by the broker.\n", __func__);
        return;

    }
    mosquitto_message_callback_set(mosq, traitement);
    // begin of a new thread 
    int status = mosquitto_loop_start(mosq);
    if(status == MOSQ_ERR_SUCCESS){
        printf("%s: dans mosquitto_loop_start success\n", __func__);
        printf("topic :%s\n", topic);
    } 
    else if(status == MOSQ_ERR_INVAL){
        printf("%s: dans mosquitto_loop_start Error parametre invalide \n", __func__);
        return;

    } 
    else if(status == MOSQ_ERR_NOT_SUPPORTED){
        printf("%s: dans mosquitto_loop_start Error parametre invalides \n", __func__);
        return;

    } 
	
    
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
        printf("%s: argument NULL donc rien a free dans mqtt_free\n", __func__);
        return;
    }
    int rc = mosquitto_disconnect(mosq);
    if(rc == MOSQ_ERR_SUCCESS)
    {
        printf("%s: dans mosquitto_disconnect Success\n", __func__);
    }
    else if(rc == MOSQ_ERR_INVAL)
    {
        printf("%s: dans mosquitto_disconnect Error parametres d'entree invalides \n", __func__);
        return;
    }
    else if (rc == MOSQ_ERR_NO_CONN	)
    {
         printf("%s: dans mosquitto_disconnect Error the client isn’t connected to a broker.", __func__);
         return;
    }

    int status = mosquitto_loop_stop(mosq, true);
     if(status == MOSQ_ERR_SUCCESS){

        printf("%s: dans mosquitto_loop_stop Success \n", __func__);

    } 
    else if(status == MOSQ_ERR_INVAL){

        printf("%s: dans mosquitto_loop_stop Error parametre invalide, peut etre qu'il n'y a pas de thread de lance (quand on veut juste publier) \n", __func__);
        
    } 
    else if(status == MOSQ_ERR_NOT_SUPPORTED){

        printf("%s: dans mosquitto_loop_stop Error thread support is not available. \n", __func__);
        
    } 
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}

