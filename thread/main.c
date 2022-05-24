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
    wiringPiSetup();
    pinMode(21,OUTPUT);

    while(1)
    {
        asm("nop;");
        digitalWrite(21,1);
        asm("nop;");
        digitalWrite(21,0);
    }

    return 0;

}
