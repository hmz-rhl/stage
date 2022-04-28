#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#include "../lib/spi-dev-lib.h"
#include "mcp3202-adc.h"

#include "../lib/expander-i2c.h"

#include <wiringPi.h>


/******************************************************************************/
int readAdc(){

	uint8_t tx[] = {0};
	uint8_t rx[ARRAY_SIZE(tx)] = {0};
	
	unsigned int reData = 0;           

	tx[0] = START_BIT;

	tx[1] = ADC_CONFIG_SGL_MODE_MSBF_CN0;

	tx[2] = DNT_CARE_BYTE;

	expander_t *exp = expander_init(0x27);

	uint8_t ancienne_config = expander_getAllPinsGPIO(exp);

	

	expander_resetOnlyPinSetOthersGPIO(exp, 4);

	spiTransfer(data);

	expander_setAndResetSomePinsGPIO(exp, ancienne_config);

	reData = (((data->rx[1] << 8) + data->rx[2]) & MSBF_MASK);
#ifdef DEBUG
	printf("The analog input value is \n");
	printf("Value at MCP3202 CH%d is: %d D : %X H \n", data->channelNo, reData, reData);
#endif
	return 0;
}

/******************************************************************************/
int main(int argc, char **argv){
	
	int retVal = 0;
	spiData *data = malloc(sizeof(spiData));

	if(argc < 3){
		printf("Usage: %s <adc_channel> <spi_dev_name>\ne.g: mcp3202 0 /dev/spidev1.0\n", argv[0]);
		exit(-1);
	}
	
	data->mode = 0;                     
	data->bits = 8;                   
	data->speed = 1000000;           
	data->delay = 0;
	sprintf(data->device, "%s", argv[2]);

	data->channelNo = atoi(argv[1]);

	if(spiInit(data)){
		perror("SPI Init failed");
		exit(-1);
	}
	
	retVal = readAdc(data);
	if(retVal < 0){
		perror("Failed to read ADC");
	}

	free(data);
	close(data->fileDescriptor);

	return retVal;               
}

 	


