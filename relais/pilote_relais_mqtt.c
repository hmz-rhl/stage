#include "../lib/expander-i2c.h"
#include "../MQTT/mqtt.h"

#include <signal.h>

// Déclaration des instances mosquitto une par topic
struct mosquitto* mosq;

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
    mqtt_free(mosq);
    printf("fin de libération de mémoire\n");
    exit(EXIT_SUCCESS);

}

int main()
{
    mosq = init_mqtt(); 
    char *const topics[4] = {"down/type_ef/open","down/type_ef/close","down/type2/open","down/type2/close"};
    
    // mosquitto_subscribe_callback_set(mosq, on_subscribe);


    mqtt_subscribe("down/type2/close",traitement, mosq);
    //mqtt_subscribe_multiple(topics,4,traitement, mosq);


    signal(SIGINT, interruption);

    printf("Abonnement avec succès\n");
    while(1)
    {
        //On s'abonne aux différents topics qui concernent les relais
        
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


