#include "mqtt.h"

void traitement(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg)
{
    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);
    if(!strcmp(msg->payload, "1")){
        printf("allumage de la led\n");
    }
    else if(!strcmp(msg->payload, "0")){
        printf("éteignage de la led\n");

    }
}

int main(int argc, char const *argv[])
{
    /* code */
    struct mosquitto* mosq = init_mqtt();
    mqtt_subscribe(argv[1], traitement, mosq);
    while(1){}
    return 0;
}
