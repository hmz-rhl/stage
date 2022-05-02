#include "mqtt.h"

int main(int argc, char const *argv[])
{
    /* code */
    struct mosquitto* mosq = init_mqtt();
    mqtt_publish(argv[1], argv[2], mosq);
    return 0;
}
