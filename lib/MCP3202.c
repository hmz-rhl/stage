#include "MCP3202.h"




void waitForSPIReady(expander_t *exp){
	
	time_t start, end;
	double attente = 0;
	start = clock();
	while(( expander_getAllPinsGPIO(exp) & (uint8_t)0b00111100 != 0b00111100 ))
	{
		end = clock();
		attente = (double)(end - start) / (double)(CLOCKS_PER_SEC);
		if(attente > 5)
		{
			perror("Erreur timeout: SPI busy tout les CS ne sont pas a 1");
			exit(EXIT_FAILURE);
		}
	}
}


/**
 * 
 * @brief  renvoi le code de conversion sur 12bitsde l'adc d'un channel precis et d'un adc precis
 * 
 * @param   channel le channel à lire 0 ou 1 
 * @param   cs le chip select de l'adc a lire (utiliser les constante de la lib expander-i2c.h (T_CS, PP_CS...))
 * 
 * @return  l'output code entre 0 et 4095 
 * 
 *  **/
int readAdc(int channel, uint8_t cs){

	unsigned int reData = -1;           
	
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

	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);

	waitForSPIReady(exp);
	if(wiringPiSPISetup(0, 2000000) < 0)
	{
		perror("Erreur de setup de SPI");
		return reData;
	}
	// cs de Temperature adc a 0 uniquement lui les autres 1 
	expander_resetPinGPIO(exp, cs);
	
	usleep(1);

	wiringPiSPIDataRW(0, data, 3);
	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);


	usleep(1); // temps necessaire pour pouvoir redemander la valeur apres. ( TCSH = 500 ns)
	expander_setPinGPIO(exp, cs);


	reData = (((data[1] << 8) + data[2]) & MSBF_MASK);

	expander_closeAndFree(exp);

#ifdef DEBUG
	printf("The analog input value is \n");
	printf("Value at MCP3202 CH%d is: %d D \n", channel, reData);
#endif
	return reData;
}


/**
 * 
 * @brief  convertie l'output code du mcp3202 en tension en volt
 * 
 * @param   code l'output code compris entre 0 et 4095
 * 
 * @return  la tension en volt convertit a partir de l'output code
 * 
 *  **/
double toVolt(int code){

	if(code < 0){

		perror("ne peut pas convertir le code de sortie en volt valeur negative");
        return code;
	}
	return code*3.3/4095;
}