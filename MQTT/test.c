#include "mqtt.h"

void traitement(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);
}

int main(int argc, char const *argv[])
{
    /* code */
    // mqtt_subscribe(argv[1], traitement);
    mqtt_publish(argv[1], "22");
    return 0;
}
