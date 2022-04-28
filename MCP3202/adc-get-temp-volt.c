#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>


#include "../lib/mcp3202-adc.h"

#include "../lib/expander-i2c.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>

// L'ADC quantifie sur 12Bits, il y a donc 4096 valeurs possible de conversion.
// On a 1C tous les 12 pas en sortie chez l'adc.
// Range du termometre: -40C - 150C ; 0,1V a 2V.

#define DEBUG
/******************************************************************************/
int readAdc(int channel){

	unsigned int reData = -1;           
	
	if(wiringPiSPISetup(0, 2000000) < 0)
	{
		perror("Erreur de setup de SPI");
		return reData;
	}
	uint8_t data[3] = {0};

	

	data[0] = START_BIT;
	if(channel == 0){
		data[1] = ADC_CONFIG_SGL_MODE_MSBF_CN0;
	}
	else{
		data[1] = ADC_CONFIG_SGL_MODE_MSBF_CN1;
	}
	data[2] = DNT_CARE_BYTE;

	expander_t *exp = expander_init(0x27);

	uint8_t ancienne_config = expander_getAllPinsGPIO(exp);


	// cs de Temperature adc a 0 uniquement lui les autres 1 
	expander_resetOnlyPinSetOthersGPIO(exp, T_CS);
	
	sleep(1);

	wiringPiSPIDataRW(0, data, 3);
	expander_setAndResetSomePinsGPIO(exp, ancienne_config);

	sleep(1); // temps necessaire pour pouvoir redemander la valeur apres. ( TCSH = 500 ns)

	reData = (((data[1] << 8) + data[2]) & MSBF_MASK);

	expander_closeAndFree(exp);

#ifdef DEBUG
	printf("The analog input value is \n");
	printf("Value at MCP3202 CH%d is: %d D \n", channel, reData);
#endif
	return reData;
}

/******************************************************************************/
int main(int argc, char **argv){

	
	int retVal = readAdc(0);
	if(retVal < 0){
		perror("Failed to read ADC");
	}

	return retVal;               
}

 	


