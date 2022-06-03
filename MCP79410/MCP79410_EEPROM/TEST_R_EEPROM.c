#include "EEPROM.h"
#include "i2c.h"
#include <time.h>
#include <stdlib.h>
#include "Utilities.h"
#include <stdio.h>
#include <stdbool.h>


void  main()
{

	I2C_Initialize(EEPROM_ADDRESS);


 	// Lecture du buffer actuel dans l'eeprom privé
	 for (size_t i = 0; i < 8; i++)
	 {
		 /* code */
		printf("EE%d 0x%2x. ",i, EEPROM_Read(EEPROM_PROTECTED_START + i));
	 }
	 


	// ecriture du nouveau buffer

	unsigned char test[]="aAbBcCdD";

	for (size_t i = 0; i < 8; i++)
	{
		/* code */
		EEPROM_WriteProtected(EEPROM_PROTECTED_START + i,test[i]);
	}




	// Lecture du nouveau buffer dans l'eeprom privé
	 for (size_t i = 0; i < 8; i++)
	 {
		 /* code */
		printf("EE%d 0x%2x. ",i, EEPROM_Read(EEPROM_PROTECTED_START + i));
	 }


	I2C_Close();
return;
}