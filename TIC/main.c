#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <wiringPi.h>
#include "../lib/rtc_eeprom.h"
// #include <wiringSerial.h>

#define SM_TIC_D 2

   

    struct timeval start;
    struct timeval end;

 
uint16_t eeprom_getWh()
{
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();
    uint8_t val1 = eeprom_readProtected (rtc_eeprom, 0xF0);
    uint8_t val2 = eeprom_readProtected (rtc_eeprom, 0xF1);
    uint16_t result = ((val2 << 8) | val1);

    return result; 

}

void eeprom_writeID(char *id){


	uint8_t a,b;
	if(strlen(id) != 12)
	{
		printf("Error %s: Id dépasse la taille autorisé (12 caractères 0-F)\n", __func__);
		exit(EXIT_FAILURE);
	}




// on verifie l'id
	for(int i = 0; i<12 ; i++){

		if((id[i] >= 48 && id[i]<=57) || (id[i] >= 'A' && id[i] < 'Z'))
		{
			
		}
		else
		{
		    printf("Error %s: ID incorrect, les caracteres utilisés ne sont pas corrects (0-F)\n", __func__);
		    exit(EXIT_FAILURE);
		}
	}

	rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();

// on reset le compteur d'energie
	eeprom_writeProtected(rtc_eeprom, 0xF0, 0x00);
	eeprom_writeProtected(rtc_eeprom, 0xF1, 0x00);

	char id2[15] = "0x";
    strcat(id2,id);
    long value = strtol( id2,NULL, 16 );
	// on ecrit l'id
	for (size_t i = 0; i < 6; i++)
	{
		/* code */
		eeprom_writeProtected(rtc_eeprom, 0xF2 + i, (value >> 8*i) & 0xFF);
	}

	// eeprom_printProtected(rtc_eeprom);
	
	rtc_eeprom_closeAndFree(rtc_eeprom);

}

void S0_interrupt(void){
    // registre   F1 F0     F1 F0        F1 F0     F1 F0   
    // passé de 0x00 00 a 0x00 01 puis 0x00 FF a 0x01 00

    // 1) on incrémente le bite de poids faible a chaque intérruption
    // 2) Quand il atteint la valeur max ( 0xFF ) on le passe a zéro et on incrémente le 2e bite
    // 3) On remet a 0 le 2e registre quand il atteind 0xFF et que le premier atteint 0xFF


    gettimeofday(&end, NULL);
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();
    // time_t temps;
    double temps;
    

    eeprom_printProtected(rtc_eeprom);
    
    if(eeprom_readProtected(rtc_eeprom,0xF0) == 0xFF)
    {
        eeprom_writeProtected(rtc_eeprom, 0xF0, 0x00);
       
        if(eeprom_readProtected(rtc_eeprom,0xF1) == 0xFF)
        {
            eeprom_writeProtected(rtc_eeprom, 0xF1, 0x00);
        }
        else
        {
            uint8_t val_F1 = eeprom_readProtected(rtc_eeprom,0xF1);
            eeprom_writeProtected(rtc_eeprom, 0xF1, val_F1 + 1);
        }

    }
    
    else
    {
       uint8_t val_F0 = eeprom_readProtected(rtc_eeprom,0xF0);
       eeprom_writeProtected(rtc_eeprom, 0xF0, val_F0 + 1);
    }

    temps = (end.tv_sec - start.tv_sec) + 1e-6*(end.tv_usec - start.tv_usec);
    printf("temps : %ld \n",temps);

    rtc_eeprom_closeAndFree(rtc_eeprom);

    printf("energie : %d Wh\n", eeprom_getWh());

    printf("puissance : %lf W\n", 1.0/(temps/3600.0));
    start = end;

}

int main(int argc, char const *argv[])
{
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();
    eeprom_printProtected(rtc_eeprom);

    eeprom_writeID("123ABCDEF456");
    // eeprom_writeProtected(rtc_eeprom, 0xF0, 0xFA);
    eeprom_printProtected(rtc_eeprom);
    // rtc_printTime(rtc_eeprom);
    // printf("OSC running : %d\n", rtc_isOscRunning(rtc_eeprom));
    // rtc_startClock(rtc_eeprom);

    uint64_t id = 0;
    uint64_t id2 = 0;
	for (size_t i = 0; i < 3; i++)
	{
		/* code */
		id = id + (eeprom_readProtected(rtc_eeprom, 0xF2 + i) << (8*i));
        printf("id: %X \n", id);
	}
    	for (size_t i = 0; i < 3; i++)
	{
		/* code */
		id2 = id2 + (eeprom_readProtected(rtc_eeprom, 0xF5 + i) << (8*i));
        printf("id2: %X \n", id2);
	}
    
    id = id + (id2 << (24));
    char str_id[13];

    printf("%X \n", id);
    
	sprintf(str_id, "%.012X \n", id);

	printf("ID :%s\n",str_id);
    rtc_eeprom_closeAndFree(rtc_eeprom);

    if(wiringPiSetup() < 0)
	{
		fprintf(stderr, "fonction %s: Unable to set up WiringPi: %s\n", __func__, strerror(errno));
		exit(EXIT_FAILURE);
	}
    pinMode(SM_TIC_D, INPUT);

	pullUpDnControl(SM_TIC_D, PUD_OFF);

    //wiringPiISR (SM_TIC_D, INT_EDGE_FALLING,  &interruption);
    
	


    while(1);

    return 0;
}
