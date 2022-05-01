#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>


#include "../lib/MCP3202.h"



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

	while(1){

		sleep(1);
		printf("temp : %.2f°C\n",	toDegres(readAdc(0,T_CS)));
		printf("PP : %.2fV\n",		toVolt(readAdc(0,PP_CS)));
		printf("CP : %.2fV\n\n\n",		toVolt(readAdc(0,CP_CS)));
	}
	return EXIT_SUCCESS;               
}

 	


