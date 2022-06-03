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
	printf("EE0 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START));
	printf("EE1 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+1));
	printf("EE2 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+2));
	printf("EE3 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+3));
	printf("EE4 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+4));
    printf("EE1 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+5));
	printf("EE2 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+6));
	printf("EE3 0x%2x. \n", EEPROM_Read(EEPROM_PROTECTED_START+7));


	// ecriture du nouveau buffer

	unsigned char test[]="aAbBcCdD";

	EEPROM_WriteProtected(EEPROM_PROTECTED_START,test);


	// Lecture du nouveau buffer dans l'eeprom privé

    printf("EE0 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START));
	printf("EE1 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+1));
	printf("EE2 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+2));
	printf("EE3 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+3));
	printf("EE4 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+4));
    printf("EE1 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+5));
	printf("EE2 0x%2x. ", EEPROM_Read(EEPROM_PROTECTED_START+6));
	printf("EE3 0x%2x. \n", EEPROM_Read(EEPROM_PROTECTED_START+7));


	I2C_Close();
return;
}