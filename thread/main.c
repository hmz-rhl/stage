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

void alarmWakeup(int sig_num);


int main(int argc, char *argv[])
{
    unsigned int j;

    wiringPiSetup();//use the physical pin numbers on the P1 connector
    pinMode(23,OUTPUT);

    signal(SIGALRM, alarmWakeup);   
    ualarm(5000, 5000);


    while(1)
    {
    }

    return 0;

}

void alarmWakeup(int sig_num)
{
    if(sig_num == SIGALRM)
    {
        digitalWrite(23, digitalRead(23));
    }

}
