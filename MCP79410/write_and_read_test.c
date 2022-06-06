#include "rtc_eeprom.h"



int main(int argc, char const *argv[])
{
    /* code */
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();

    // eeprom_print(rtc_eeprom);
    // eeprom_printProtected(rtc_eeprom);

    // eeprom_write(rtc_eeprom, 0x0F, eeprom_read(rtc_eeprom,0x0F) + 1);
    // eeprom_writeProtected(rtc_eeprom, 0xF0, eeprom_readProtected(rtc_eeprom,0xF0) + 1);

    // eeprom_print(rtc_eeprom);
    // eeprom_printProtected(rtc_eeprom);
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
    rtc_startClock(rtc_eeprom);
    printf("PWRFAIL : %d\n", rtc_isPwrFail(rtc_eeprom));
    printf("OSCRUN : %d\n", rtc_isOscRunning(rtc_eeprom));
    printf("VBATEN : %d\n", rtc_isVbatEnabled(rtc_eeprom));

    rtc_enableVbat(rtc_eeprom);

    printf("PWRFAIL : %d\n", rtc_isPwrFail(rtc_eeprom));
    printf("OSCRUN : %d\n", rtc_isOscRunning(rtc_eeprom));
    printf("VBATEN : %d\n", rtc_isVbatEnabled(rtc_eeprom));
    
    printf("Date : %X/%X/%X, Time : %X:%X:%X\n", rtc_readDate(rtc_eeprom), rtc_readMonth(rtc_eeprom), rtc_readYear(rtc_eeprom), rtc_readHours(rtc_eeprom), rtc_readMinutes(rtc_eeprom), rtc_readSeconds(rtc_eeprom));
    
    
    
    

    // printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));
    // rtc_writeSeconds(rtc_eeprom, 0x0F);
    // printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));
    // rtc_startClock(rtc_eeprom);
    // sleep(2);
    // printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));
    //     sleep(2);
    // printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));

    rtc_eeprom_closeAndFree(rtc_eeprom);
    
    return 0;
}
