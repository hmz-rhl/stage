/**
 * @file EEPROM.h
 * @author Hamza RAHAL
 * @brief header du driver pour piloter MCP79410
 * @version 0.1
 * @date 2022-06-04
 * 
 * @copyright Saemload (c) 2022
 * 
 */

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
#define RTC_ADDRESS 0x6F

typedef struct{

    int eeprom_fd; // pour l'eeprom
    int rtc_fd; // pour la rtc
    uint8_t buf[128];
    // TODO : a mettre en place dans les fonction
    int error;

}rtc_eeprom_t;

rtc_eeprom_t *rtc_eeprom_init(void);



uint8_t eeprom_read(rtc_eeprom_t* rtc_eeprom, uint8_t reg);
void eeprom_write(rtc_eeprom_t* rtc_eeprom, uint8_t reg, uint8_t val);

uint8_t eeprom_readProtected(rtc_eeprom_t* rtc_eeprom, uint8_t reg);
void eeprom_writeProtected(rtc_eeprom_t* rtc_eeprom, uint8_t reg, uint8_t val);

uint8_t eeprom_readStatus(rtc_eeprom_t *rtc_eeprom);

void eeprom_print(rtc_eeprom_t *rtc_eeprom);
void eeprom_printProtected(rtc_eeprom_t *rtc_eeprom);

// uint8_t rtc_readSeconds(rtc_eeprom_t* rtc_eeprom);
// void rtc_writeSeconds(rtc_eeprom_t* rtc_eeprom, uint8_t val);

void rtc_eeprom_closeAndFree(rtc_eeprom_t *rtc_eeprom);

