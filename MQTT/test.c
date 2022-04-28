#include "mqtt.h"

void traitement(void (*on_message)(struct mosquitto *, void* , const struct mosquitto_message*)
{
    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);
}
void triple(int num)
{
    printf("%d\n",3*num);
}

void moitie(int num)
{
    printf("%f\n", num/2.0);
}

void fonctionCeption(void (*fun)(int))
{
    fun(10);
}
void utiliseFonction(void (*fun)(int), int ii )
{
    fonctionCeption(fun);
    printf("fin de la fonction uiliseFonction\n");
}


int main(int argc, char const *argv[])
{
    /* code */
    mqtt_subscribe(argv[1], traitement);
    //mqtt_publish(argv[1], "25");
    //utiliseFonction(moitie, 2);
    return 0;
}
