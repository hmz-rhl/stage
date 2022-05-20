#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPi.h>
#include <time.h>
#define LED_Pin 29
int main(int argc, char const *argv[])
{
    /* code */
    timespec t = {0,0};
    if(wiringPiSetup() < 0)
	{
		fprintf(stderr, "fonction %s: Unable to set up: %s\n", __func__, strerror(errno));
	}
    pinMode(LED_Pin, OUTPUT);
    while(1){

        t.tv_nsec = 900;
        digitalWrite(LED_Pin, 1);
        nanosleep(t);
        t.tv_nsec = 1800;
        digitalWrite(LED_Pin, 0);
        nanosleep(t);    
    }

    return 0;
}
