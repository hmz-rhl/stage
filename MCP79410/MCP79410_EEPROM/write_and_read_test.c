#include "eeprom.h"



int main(int argc, char const *argv[])
{
    /* code */
    eeprom_t *eeprom = eeprom_init();

    eeprom_print(eeprom);
    eeprom_printProtected(eeprom);

    eeprom_write(eeprom, 0x0F, eeprom_read(eeprom,0x0F) + 1);
    eeprom_writeProtected(eeprom, 0xF0, eeprom_readProtected(eeprom,0xF0) + 1);

    eeprom_print(eeprom);
    eeprom_printProtected(eeprom);

    eeprom_closeAndFree(eeprom);
    
    return 0;
}
