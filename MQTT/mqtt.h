//Hicham GHANEM et Raphaël GANDUS


/*
*   Explication pour l'utilisation de la librairie mqtt :
        - inclure la librairie "mqtt.h" que nous avons créée (il faut avoir les fichiers mqtt.h, mqtt.c et topic.h dans le répertoire de travail)
        - créer une variable d'instance de type struct mosquitto* égale à la fonction init_mqtt()
        - pour publier sur un topic, appeler la fonction mqtt_publish(*topic*, *message*, *variable d'instance*)
        - pour souscrire à un topic, appeler la fonction mqtt_subscribe(*topic*, *fonction de traitement*, *variable d'instance*)
          un exemple de fonction de traitement que vous pouvez utiliser :
                    void traitement(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg)
                    {
                        printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);
                        if(!strcmp(msg->payload, "1"))
                        {
                            printf("allumage de la led\n");
                        }
                        else if(!strcmp(msg->payload, "0"))
                        {
                            printf("éteignage de la led\n");
                        }
                    }
*           Vous devrez créer cette fonction de traitement selon vos besoins dans votre main. Elle servira notamment à récuperer la valeur renvoyée par le broker
*/



// Librairies 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mosquitto.h>



//Déclarations des fonctions mqtt

struct mosquitto* init_mqtt();
void mqtt_publish(char *topic, char *message, struct mosquitto* mosq);
void mqtt_subscribe(char *topic, void (*on_message)(struct mosquitto *, void* , const struct mosquitto_message*), struct mosquitto* mosq);
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
void mqtt_free(struct mosquitto* mosq);

