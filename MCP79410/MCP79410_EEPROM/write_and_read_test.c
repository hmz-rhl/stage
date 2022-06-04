#include "eeprom.h"



int main(int argc, char const *argv[])
{
    /* code */
    eeprom_t *eeprom = eeprom_init();

    
    printf("on lit le contenue de 0x00: %02X\n", eeprom_read(eeprom, 0x00));
    
    printf("On incremente le contenu de 0x00\n");
    eeprom_write(eeprom, 0x00, eeprom->buf[0] + 1);

    printf("on lit le contenue de 0x00: %02X\n", eeprom_read(eeprom, 0x00));

    for (size_t i = 0; i < 8; i++)
    {
        /* code */
        printf("%02X : %02X\n", 0xF0+i, eeprom_readProtected(eeprom, 0XF0 + i));
    }

    printf("On ecrit 0xAA dans 0xF7\n");
    eeprom_writeProtected(eeprom, 0xF7, 0xAB);
    printf("status : %02X\n", eeprom_readStatus(eeprom));


    for (size_t i = 0; i < 8; i++)
    {
        /* code */
        printf("%02X : %02X\n", 0xF0+i, eeprom_readProtected(eeprom, 0XF0 + i));
    }

    eeprom_closeAndFree(eeprom);
    
    return 0;
}
