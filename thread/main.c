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
#include <timer.h>

int i = 0;
void alarmWakeup(int sig_num);


int main(int argc, char *argv[])
{
    unsigned int j;

    wiringPiSetup();
    pinMode(23,OUTPUT);
    struct repeating_timer t;
    add_repeating_timer_ms(500, alarmWakeup, NULL, &t);
    while(1)
    {
        tight_loop_contents();
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
