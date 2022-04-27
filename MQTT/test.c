#include "mqtt.h"

void traitement(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);
}

int main(int argc, char const *argv[])
{
    /* code */
    mqtt_subscribe("up/charge/temp", traitement);
    return 0;
}
