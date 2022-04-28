#include "mqtt.h"

void traitement(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);
}
void triple(int num)
{
    printf("%d\n",3*num);
}

void moitie(int num)
{
    printf("%f\n", num/2);
}

void utiliseFonction(void (*fun)(int) )
{
    fun(10);
    printf("fin de la fonction uiliseFonction\n");
}

int main(int argc, char const *argv[])
{
    /* code */
    //mqtt_subscribe(argv[1], traitement);
    //mqtt_publish(argv[1], "25");
    utiliseFonction(moitie);
    return 0;
}
