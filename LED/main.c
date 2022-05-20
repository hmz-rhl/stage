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
    int i = 0;
    /* code */

    if(wiringPiSetup() < 0)
	{
		fprintf(stderr, "fonction %s: Unable to set up: %s\n", __func__, strerror(errno));
	}
    pinMode(LED_Pin, OUTPUT);
    while(1){


        digitalWrite(LED_Pin, 0);
        while(i++<500/4)
        {
            asm("nop");
        }
        digitalWrite(LED_Pin, 1);
        i=0;
        while(i++<1000/4) 
        {
            asm("nop");
        }
            
    }

    return 0;
}
