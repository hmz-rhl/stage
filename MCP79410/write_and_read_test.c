#include "../lib/rtc_eeprom.h"



int main(int argc, char const *argv[])
{
    /* code */
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();
    eeprom_printProtected(rtc_eeprom); // On lit l'EEPROM Protege
    char str_id[100];
	long id = 0;
    long id2 = 0;

	for (size_t i = 0; i < 3; i++)
	{
		/* code */
		id = id + (eeprom_readProtected(rtc_eeprom, 0xF2 + i) << (8*i));
        //printf("id: %X \n", id);
	}
    	for (size_t i = 0; i < 3; i++)
	{
		/* code */
		id2 = id2 + (eeprom_readProtected(rtc_eeprom, 0xF5 + i) << (8*i));
        //printf("id2: %X \n", id2);
	}
    
    
    char str_id1[7];
    char str_id2[7];


    
	sprintf(str_id1, "%.06X", id);
    sprintf(str_id2, "%.06X", id2);
    strcat(str_id,str_id2);
    strcat(str_id,str_id1);
	printf("%s\n", str_id);
	rtc_eeprom_closeAndFree(rtc_eeprom);
    // eeprom_write(rtc_eeprom, 0x0F, eeprom_read(rtc_eeprom,0x0F) + 1); // On incremente le registre 0x0F l'EEPROM 
    // eeprom_writeProtected(rtc_eeprom, 0xF0, eeprom_readProtected(rtc_eeprom,0xF0) + 1); // On incremente le registre 0xF0 de l'EEPROM Protege

    // eeprom_print(rtc_eeprom); // On affiche l'EEPROM 
    // eeprom_printProtected(rtc_eeprom); // ON affiche l'EEPROM Protege


    // eeprom_setAll(rtc_eeprom);
    // rtc_stopClock(rtc_eeprom);
    // rtc_writeMinutes(rtc_eeprom, 23);
    // rtc_writeHours(rtc_eeprom, 2);
    // rtc_writeDate(rtc_eeprom, 06);
    // rtc_writeMonth(rtc_eeprom, 6);
    // rtc_writeYear(rtc_eeprom, 22);
    // rtc_startClock(rtc_eeprom);

    // eeprom_print(rtc_eeprom);
    // eeprom_printProtected(rtc_eeprom);
    // eeprom_setAll(rtc_eeprom);
    // rtc_startClock(rtc_eeprom);
    // rtc_enableExtOsc(rtc_eeprom);
    // printf("PWRFAIL : %d\n", rtc_isPwrFail(rtc_eeprom));
    // printf("OSCRUN : %d\n", rtc_isOscRunning(rtc_eeprom));
    // printf("VBATEN : %d\n", rtc_isVbatEnabled(rtc_eeprom));

    // rtc_printDate(rtc_eeprom);    
    // rtc_printTime(rtc_eeprom);    
    
    
    

    // printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));
    // rtc_writeSeconds(rtc_eeprom, 0x0F);
    // printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));
    // rtc_startClock(rtc_eeprom);
    // sleep(2);
    // printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));
    //     sleep(2);
    // printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));

    rtc_eeprom_closeAndFree(rtc_eeprom); // On libère la mémoire allouée
    
    return 0;
}
