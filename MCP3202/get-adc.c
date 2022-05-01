#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <string.h>


#include "../lib/MCP3202.h"
#include "../MQTT/mqtt.h"



// L'ADC quantifie sur 12Bits, il y a donc 4096 valeurs possible de conversion.
// On a 1C tous les 12 pas en bit en sortie chez l'adc.
// Range du termometre: -40C -- 150C ; 0,1V a 2V.

#define DEBUG
/******************************************************************************/



double toDegres(int tension){

	if(tension < 0){

		perror("ne peut pas convertir une tension negative en degres");
		return tension;
	}
	return (toMillivolt(tension)-500)/10;
}


/******************************************************************************/
int main(int argc, char **argv){
	double temp, cp, pp;
	int TEMP, PP, CP;

	struct mosquitto* mosq = init_mqtt();

	while(1){

		temp = toDegres(readAdc(0,T_CS));
		pp = toVolt(readAdc(0,PP_CS));
		cp = toVolt(readAdc(0,CP_CS));

		sleep(2);
		printf("temp : %.2f°C\n",	temp);
		printf("pp adc : %.2fV\n",		pp);
		printf("cp adc : %.2fV\n\n\n",	cp);

		TEMP = (int)temp;
		mqtt_publish("up/value/temp_test", itoa(TEMP), mosq);
		
        if (cp > 9.5){

            CP = 12;
		}
        else if( cp >= 7.5){

            CP = 9;
		}
        else if( cp >= 4.5){

            CP = 6;
		}
        else if( cp >= 1.5){

            CP = 3;
		}
        else if( cp > -1.5){

            CP = 0;
		}
        else{

			CP = -12;
		}
		mqtt_publish("up/value/cp_test", itoa(CP), mosq);

		if (pp > 9.5){

            PP = 80;
		}
        else if( pp >= 7.5){

            PP = 63;
		}
        else if( pp >= 4.5){

            PP = 32;
		}
        else if( pp >= 1.5){

            PP = 20;
		}
        else if( pp > -1.5){

            PP = 13;
		}
        else{
			
			PP = 6;
		}
		mqtt_publish("up/value/pp_test", itoa(PP), mosq);

		


	
	}
	return EXIT_SUCCESS;               
}

 	


