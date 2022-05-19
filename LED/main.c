#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPi.h>

#define LED_Pin 29
int main(int argc, char const *argv[])
{
    /* code */
    pinMode(LED_Pin, OUTPUT);
    while(1){

        digitalWrite(LED_Pin, 1);
        usleep(80);
        digitalWrite(LED_Pin, 0);
        usleep(45);
    
    }

    return 0;
}
