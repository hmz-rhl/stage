#include "eeprom.h"



int main(int argc, char const *argv[])
{
    /* code */
    eeprom_t *eeprom = eeprom_init();

    
    printf("on lit le contenue de 0x00: %02X\n", eeprom_read(eeprom, 0x00));
    
    printf("On incremente le contenu de 0x00\n");
    eeprom_write(eeprom, 0x00, eeprom->buf[0] + 1);

    printf("on lit le contenue de 0x00: %02X\n", eeprom_read(eeprom, 0x00));

    
    printf("\n\non lit le contenue de 0xF7: %02X\n", eeprom_readProtected(eeprom, 0xF7));
    
    printf("On incremente le contenu de 0xF7\n");
    eeprom_writeProtected(eeprom, 0xF7, 0xAB);

    printf("on lit le contenue de 0xF7: %02X\n", eeprom_readProtected(eeprom, 0xF7));


    eeprom_closeAndFree(eeprom);
    
    return 0;
}
