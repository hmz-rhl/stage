//Hicham GHANEM et Raphaël GANDUS

// Librairies 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mosquitto.h>

#include "topic.h"

//Déclarations des fonctions mqtt
struct mosquitto* init_mqtt();
void mqtt_publish(char *topic, char *message, struct mosquitto* mosq);
void mqtt_subscribe(char *topic, void (*on_message)(struct mosquitto *, void* , const struct mosquitto_message*), struct mosquitto* mosq);
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
int test_topic(char *topic);
void mqtt_free(struct mosquitto* mosq);

