//Hicham GHANEM et Raphaël GANDUS


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mosquitto.h>

#include "topic.h"

struct mosquitto* init_mqtt();
void mqtt_publish(char *topic, char *message);
//void mqtt_subscribe(char *topic, void (*call)(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg));
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
int test_topic(char *topic);


