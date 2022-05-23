#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <wiringPi.h> // compilation ajouter -lwiringPi
#include <pthread.h> // compilation ajouter -lptrhread
#include <softPwm.h>


int i = 0;



int main(int argc, char *argv[])
{


    while(1)
    {
        printf("%d\n", clock());
        sleep(1);
    }

    return 0;

}

void alarmWakeup(struct repetaing_timer *t)
{

      
    digitalWrite(23, i);
    i = 1-i;
    printf("alarm!\n");
    return true;

}
