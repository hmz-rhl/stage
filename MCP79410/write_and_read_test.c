#include "rtc_eeprom.h"



int main(int argc, char const *argv[])
{
    /* code */
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init(); // Initialisation de la communication

    eeprom_print(rtc_eeprom); // On lit l'EEPROM
    eeprom_printProtected(rtc_eeprom); // On lit l'EEPROM Protege

    eeprom_write(rtc_eeprom, 0x0F, eeprom_read(rtc_eeprom,0x0F) + 1); // On ecrit 0x0F dans l'adresse n+1 de l'EEPROM 
    eeprom_writeProtected(rtc_eeprom, 0xF0, eeprom_readProtected(rtc_eeprom,0xF0) + 1); // On ecrit dans l'EEPROM Protege

    eeprom_print(rtc_eeprom); // On affiche l'EEPROM 
    eeprom_printProtected(rtc_eeprom); // ON affiche l'EEPROM Protege


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

    rtc_eeprom_closeAndFree(rtc_eeprom); // On lib
    
    return 0;
}
