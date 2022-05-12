#include "../lib/expander-i2c.h"
#include "../MQTT/mqtt.h"

#include <signal.h>

// Déclaration des instances mosquitto une par topic

struct mosquitto* m;
//Declaration des fonctions de traitement 
void traitement(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg);
void traitement_ef_open(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg);
void traitement_ef_close(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg);
void traitement_type2_close(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg);
void traitement_type2_open(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg);
// void on_subscribe(struct mosquitto *mosq, void* obj, , const struct mosquitto_message* msg);

void interruption(int n)
{

    printf("interruption on free les instances\n");
    mqtt_free(m);
    printf("fin de libération de mémoire\n");
    exit(EXIT_SUCCESS);

}

int main()
{
    struct mosquitto* mosq = init_mqtt();
    m = mosq;
    char *const topics[4] = {"down/type_ef/open","down/type_ef/close","down/type2/open","down/type2/close"};
    
    // mosquitto_subscribe_callback_set(mosq, on_subscribe);


    //mqtt_subscribe("down/type2/close",traitement, mosq);
    //mqtt_subscribe_multiple(topics,4,traitement, mosq);


    signal(SIGINT, interruption);

    int debug = mosquitto_subscribe(mosq, NULL, "down/type_ef/open", 0);
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
    // int status = mosquitto_loop_start(mosq);
    // if(status == MOSQ_ERR_SUCCESS){
    //     printf("topic :%s\n", topic);
    //     printf("%s: dans mosquitto_loop_start success\n", __func__);
    // } 
    // else if(status == MOSQ_ERR_INVAL){
    //     printf("%s: dans mosquitto_loop_start Error parametre invalide \n", __func__);
    //     return;

    // } 
    // else if(status == MOSQ_ERR_NOT_SUPPORTED){
    //     printf("%s: dans mosquitto_loop_start Error parametre invalides \n", __func__);
    //     return;

    // } 

    printf("Abonnement avec succès\n");
    while(1)
    {
        //On s'abonne aux différents topics qui concernent les relais
        mosquitto_loop(mosq, 0, 1);
    }
}



void traitement(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg)
{
    expander_t* expander = expander_init(0x26); //Pour les relais

    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);

    if(!strcmp(msg->topic,"down/type_ef/open")){

        expander_resetPinGPIO(expander, 2); 
        printf("Le relais de la prise E/F est ouvert\n");
    }
    else if(!strcmp(msg->topic,"down/type_ef/close")){

        expander_setPinGPIO(expander, 2);
        printf("Le relais de la prise E/F est ferme\n");
    }

    else if(!strcmp(msg->topic,"down/type2/close")){

        if(!strcmp(msg->payload, "1")){

            expander_setPinGPIO(expander, 0);
            expander_resetPinGPIO(expander, 1);

            printf("Les relais N et L1 de la prise type 2 sont fermes\n");
        }
        else if(!strcmp(msg->payload, "3")){
            
            expander_setPinGPIO(expander, 0);
            expander_setPinGPIO(expander, 1);

            printf("Les relais N, L2 et L3 de la prise type 2 sont fermes\n");

        }

    }

    else if(!strcmp(msg->topic,"down/type2/open")){

        if(!strcmp(msg->payload, "1")){
            expander_resetPinGPIO(expander, 0);
            // expander_resetPinGPIO(expander, 1);
            printf("Les relais N et L1 de la prise type 2 sont ouverts\n");
        }
        else if(!strcmp(msg->payload, "3")){
            expander_resetPinGPIO(expander, 0);
            expander_resetPinGPIO(expander, 1);
            printf("Les relais L2 et L3 de la prise type 2 sont ouvert\n");

        }
    
    }
    
    expander_closeAndFree(expander);
}

void traitement_ef_open(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg)
{
    expander_t* expander = expander_init(0x26); //Pour les relais

    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);
    
    expander_resetPinGPIO(expander, 2); 
    printf("Le relai de la prise E/F est ouvert\n");

    expander_closeAndFree(expander);
}


void traitement_ef_close(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg)
{
    expander_t* expander = expander_init(0x26); //Pour les relais

    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);

    expander_setPinGPIO(expander, 2);
    printf("Le relai de la prise E/F est ferme\n");

    expander_closeAndFree(expander);
}

        

void traitement_type2_close(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg)
{
    expander_t* expander = expander_init(0x26); //Pour les relais

    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);
    if(!strcmp(msg->payload, "1")){
        expander_setPinGPIO(expander, 0);
        expander_resetPinGPIO(expander, 1);

        printf("Les relais N et L1 de la prise type 2 sont fermes\n");
    }
    else if(!strcmp(msg->payload, "3")){
        
        expander_setPinGPIO(expander, 0);
        expander_setPinGPIO(expander, 1);

        printf("Les relais N, L2 et L3 de la prise type 2 sont fermes\n");

    }
    expander_closeAndFree(expander);
}

void traitement_type2_open(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg)
{
    expander_t* expander = expander_init(0x26); //Pour les relais

    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);
    if(!strcmp(msg->payload, "1")){
        expander_resetPinGPIO(expander, 0);
        // expander_resetPinGPIO(expander, 1);
        printf("Les relais N et L1 de la prise type 2 sont ouverts\n");
    }
    else if(!strcmp(msg->payload, "3")){
        expander_resetPinGPIO(expander, 0);
        expander_resetPinGPIO(expander, 1);
        printf("Les relais L2 et L3 de la prise type 2 sont ouvert\n");

    }
    expander_closeAndFree(expander);
}


// void on_subscribe(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg)
// {

//     printf("Abonnement du topic %s: %s\n", msg->topic, (char *) msg->payload);

// }


