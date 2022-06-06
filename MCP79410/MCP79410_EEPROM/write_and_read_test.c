#include "rtc_eeprom.h"



int main(int argc, char const *argv[])
{
    /* code */
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();

    // eeprom_print(rtc_eeprom);
    // eeprom_printProtected(rtc_eeprom);

    // eeprom_write(rtc_eeprom, 0x0F, eeprom_read(rtc_eeprom,0x0F) + 1);
    // eeprom_writeProtected(rtc_eeprom, 0xF0, eeprom_readProtected(rtc_eeprom,0xF0) + 1);

    eeprom_print(rtc_eeprom);
    eeprom_printProtected(rtc_eeprom);
    eeprom_setAll(rtc_eeprom);
    rtc_stopClock(rtc_eeprom);
    rtc_writeMinutes(rtc_eeprom, 23);
    rtc_writeHours(rtc_eeprom, 2);
    rtc_writeDate(rtc_eeprom, 06);
    rtc_writeMonth(rtc_eeprom, 6);
    rtc_writeYear(rtc_eeprom, 2022);
    rtc_startClock(rtc_eeprom);
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
