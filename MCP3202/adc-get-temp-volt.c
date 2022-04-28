#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#include "../lib/spi-dev-lib.h"
#include "mcp3202-adc.h"

#include "../lib/expander-i2c.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define DEBUG
/******************************************************************************/
int readAdc(){

	unsigned int reData = 0;           
	
	if(wiringPiSPISetup(0, 2000000) < 0)
	{
		printf("Erreur de setup de SPI dans %s", __func__);
		return reData;
	}
	uint8_t data[] = {0};

	

	data[0] = START_BIT;

	data[1] = ADC_CONFIG_SGL_MODE_MSBF_CN0;

	data[2] = DNT_CARE_BYTE;

	expander_t *exp = expander_init(0x27);

	uint8_t ancienne_config = expander_getAllPinsGPIO(exp);


	// cs de Temperature adc a 0 uniquement lui les autres 1 
	expander_resetOnlyPinSetOthersGPIO(exp, 4);
	sleep(1);

	wiringPiSPIDataRW(0, data, 3);

	expander_setAndResetSomePinsGPIO(exp, ancienne_config);

	reData = (((data[1] << 8) + data[2]) & MSBF_MASK);

#ifdef DEBUG
	printf("The analog input value is \n");
	printf("Value at MCP3202 CH0 is: %d D \n", data);
#endif
	return reData;
}

/******************************************************************************/
int main(int argc, char **argv){

	
	int retVal = readAdc(data);
	if(retVal < 0){
		perror("Failed to read ADC");
	}

	return retVal;               
}

 	


