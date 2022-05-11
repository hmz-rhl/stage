#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <signal.h>


#include "../lib/MCP3202.h"
#include "../MQTT/mqtt.h"



// L'ADC quantifie sur 12Bits, il y a donc 4096 valeurs possible de conversion.
// On a 1C tous les 12 pas en bit en sortie chez l'adc.
// Range du termometre: -40C -- 150C ; 0,1V a 2V.


/******************************************************************************/

struct mosquitto* mosq;

double toDegres(int tension){

	if(tension < 0){

		printf("%s: ne peut pas convertir une tension negative en degres", __func__);
		return tension;
	}
	return (toMillivolt(tension)-500)/10;
}

void interruption(int n)
{
	
	printf("%S: interruption on free mosq\n", __func__);
	
	mqtt_free(mosq);
}

/******************************************************************************/
int main(int argc, char **argv){
	
	float temp, cp, pp;
	int TEMP, PP, CP;
	char str_temp[100], str_cp[100],  str_pp[100];

	signal(SIGINT, interruption);
	int i = 0;

	while(1){

		printf("%s: initialisation d'un client mqtt\n\n");
		mosq = init_mqtt();
		printf("%s: Lecture de Temperature\n", __func__);
		temp = toDegres(readAdc(0,T_CS));
		printf("%s: Lecture de PP\n", __func__);
		pp = toVolt(readAdc(0,PP_CS));
		printf("%s: Lecture de CP\n", __func__);
		cp = toVolt(readAdc(0,CP_CS));

		TEMP = (int)temp;

			printf("temp %d°C\n", TEMP);
			printf("cp %lfV\n", cp*4);
			printf("pp %lfV\n\n", pp);


		sprintf(str_temp, "%d", TEMP);

		printf("%s: Publication de Temperature\n",__func__);
		mqtt_publish("up/value/temp", str_temp, mosq);
		
        if (cp*4.0 > 9.5){

            CP = 12;
		}
        else if( cp*4.0 >= 7.5){

            CP = 9;
		}
        else if( cp*4.0 >= 4.5){

            CP = 6;
		}
        else if( cp*4.0 >= 1.5){

            CP = 3;
		}
        else if( cp*4.0 > -1.5){

            CP = 0;
		}
        else{

			CP = -12;
		}

		sprintf(str_cp, "%d", CP);

		printf("%s: Publication de CP\n",__func__);
		mqtt_publish("up/value/cp", str_cp, mosq);

		if (pp < 0.58){

            PP = 80;
		}
        else if( pp < 0.9 ){

            PP = 63;
		}
        else if( pp < 1.5 ){

            PP = 32;
		}
        else if( pp < 2.2 ){

            PP = 20;
		}
        else if( pp < 2.6 ){

            PP = 13;
		}
        else{
			
            PP = 6;
		}
		sprintf(str_pp, "%d", PP);

		printf("%s: Publication de PP\n",__func__);
		mqtt_publish("up/value/pp", str_pp, mosq);

		printf("%s: liberation de l'instance mosq\n",__func__);
		mqtt_free(mosq);

		
		printf("%s: On sleep pour 4s\n",__func__);
		sleep(4);

	
	}
	return EXIT_SUCCESS;               
}

 	


