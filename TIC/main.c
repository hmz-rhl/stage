#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
// #include <wiringSerial.h>

void interruption(void){

    printf("donnée !\n");

}
int main(int argc, char const *argv[])
{

    if(wiringPiSetup() < 0)
	{
		fprintf(stderr, "fonction %s: Unable to set up WiringPi: %s\n", __func__, strerror(errno));
		exit(EXIT_FAILURE);
	}
    pinMode(2, INPUT);
    
	pullUpDnControl(PP_IN, PUD_OFF);

    wiringPiISR (2, INT_EDGE_BOTH,  &interruption);

    while(1);

    return 0;
}
