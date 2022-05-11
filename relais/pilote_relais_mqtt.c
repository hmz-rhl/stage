#include "../lib/expander-i2c.h"
#include "../MQTT/mqtt.h"

#include <signal.h>

// Déclaration des instances mosquitto une par topic
struct mosquitto* mosq1;
struct mosquitto* mosq2;
struct mosquitto* mosq3;
struct mosquitto* mosq4;

//Declaration des fonctions de traitement 
void traitement_ef_open(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg);
void traitement_ef_close(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg);
void traitement_type2_close(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg);
void traitement_type2_open(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg);

void interruption(int n)
{

    printf("interruption on free les instances\n");
    mqtt_free(mosq1);
    mqtt_free(mosq2);
    mqtt_free(mosq3);
    mqtt_free(mosq4);
    printf("fin de libération de mémoire\n");
    exit(EXIT_SUCCESS);

}

int main()
{
    mosq1 = init_mqtt(); 
    mosq2 = init_mqtt();
    mosq3 = init_mqtt();
    mosq4 = init_mqtt();
    signal(SIGINT, interruption);

    printf("Abonnement avec succès\n");
    while(1)
    {
        //On s'abonne aux différents topics qui concernent les relais
        mqtt_subscribe("down/type_ef/open",traitement_ef_open, mosq1);
        mqtt_subscribe("down/type_ef/close",traitement_ef_close, mosq2);
        mqtt_subscribe("down/type2/open",traitement_type2_open, mosq3);
        mqtt_subscribe("down/type2/close",traitement_type2_close, mosq4);
    }
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




