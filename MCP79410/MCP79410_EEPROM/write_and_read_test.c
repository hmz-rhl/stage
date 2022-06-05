#include "rtc_eeprom.h"



int main(int argc, char const *argv[])
{
    /* code */
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();

    eeprom_print(rtc_eeprom);
    eeprom_printProtected(rtc_eeprom);

    eeprom_write(rtc_eeprom, 0x0F, eeprom_read(rtc_eeprom,0x0F) + 1);
    eeprom_writeProtected(rtc_eeprom, 0xF0, eeprom_readProtected(rtc_eeprom,0xF0) + 1);

    eeprom_print(rtc_eeprom);
    eeprom_printProtected(rtc_eeprom);

    printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));
    rtc_writeSeconds(rtc_eeprom, 0x06);
    printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));
    sleep(2);
    printf("sec: %02X \n", rtc_readSeconds(rtc_eeprom));

    rtc_eeprom_closeAndFree(rtc_eeprom);
    
    return 0;
}
