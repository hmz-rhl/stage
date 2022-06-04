#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define EEPROM_ADDRESS 0x57

typedef struct{

    int fd;
    uint8_t buf[10];
    // TODO : a mettre en place dans les fonction
    int error;

}eeprom_t;

eeprom_t *eeprom_init(void);

uint8_t eeprom_read(eeprom_t* eeprom, uint8_t reg);
void eeprom_write(eeprom_t* eeprom, uint8_t reg, uint8_t val);

uint8_t eeprom_readProtected(eeprom_t* eeprom, uint8_t reg);
void eeprom_writeProtected(eeprom_t* eeprom, uint8_t reg, uint8_t val);

void eeprom_closeAndFree(eeprom_t* eeprom);
