#include <stdio.h>
#include <stdlib.h>

#include <mosquitto.h>

struct mosquitto* init_mqtt();
void publish(char *topic, char *message);
void subscribe(char *topic, char *message);
bool test_topic(char *topic);


