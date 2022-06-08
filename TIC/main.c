#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>
#include "../lib/rtc_eeprom.h"
// #include <wiringSerial.h>

#define SM_TIC_D 2

    time_t debut=time(NULL); // Variable de temps 1
    time_t fin=time(NULL);   // Variable de temps 2

    int test = 0; // Variable pour switcher entre debut-fin et fin_debut
 
uint16_t eeprom_getWh()
{
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();
    uint8_t val1 = eeprom_readProtected (rtc_eeprom, 0xF0);
    uint8_t val2 = eeprom_readProtected (rtc_eeprom, 0xF1);
    uint16_t result = ((val2 << 8) | val1);

    return result; 

}


void interruption(void){
    // registre   F1 F0     F1 F0        F1 F0     F1 F0   
    // passé de 0x00 00 a 0x00 01 puis 0x00 FF a 0x01 00

    // 1) on incrémente le bite de poids faible a chaque intérruption
    // 2) Quand il atteint la valeur max ( 0xFF ) on le passe a zéro et on incrémente le 2e bite
    // 3) On remet a 0 le 2e registre quand il atteind 0xFF et que le premier atteint 0xFF


    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();


    eeprom_printProtected(rtc_eeprom);

    printf("1Wh !\n");
    

    
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
    
    if(test == 0)
    {
        debut=time(NULL);
        unsigned long temps = (unsigned long) difftime( fin, debut);
        test = 1;
        printf("temps : %f\n",temps);
    }
    else
    {
        fin=time(NULL);
        unsigned long temps = (unsigned long) difftime( debut, fin);
        test = 0;
         printf("temps : %f\n",temps);
    }

    rtc_eeprom_closeAndFree(rtc_eeprom);

    printf("energie : %d Wh\n", eeprom_getWh());

    printf("puissance : %d W\n", (eeprom_getWh()/temps)*3600);

}

int main(int argc, char const *argv[])
{
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();
    eeprom_resetAllProtected(rtc_eeprom);
    eeprom_writeProtected(rtc_eeprom, 0xF0, 0xFA);
    rtc_eeprom_closeAndFree(rtc_eeprom);

    if(wiringPiSetup() < 0)
	{
		fprintf(stderr, "fonction %s: Unable to set up WiringPi: %s\n", __func__, strerror(errno));
		exit(EXIT_FAILURE);
	}
    pinMode(SM_TIC_D, INPUT);

	pullUpDnControl(SM_TIC_D, PUD_OFF);

    wiringPiISR (SM_TIC_D, INT_EDGE_FALLING,  &interruption);

    while(1);

    return 0;
}
