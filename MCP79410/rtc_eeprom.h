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
    uint8_t buf[129];
    // TODO : a mettre en place dans les fonction
    int error;

}rtc_eeprom_t;

rtc_eeprom_t *rtc_eeprom_init(void);



uint8_t eeprom_read(rtc_eeprom_t* rtc_eeprom, uint8_t reg);
void eeprom_write(rtc_eeprom_t* rtc_eeprom, uint8_t reg, uint8_t val);

uint8_t eeprom_readProtected(rtc_eeprom_t* rtc_eeprom, uint8_t reg);
void eeprom_writeProtected(rtc_eeprom_t* rtc_eeprom, uint8_t reg, uint8_t val);

void eeprom_setAll(rtc_eeprom_t* rtc_eeprom);

uint8_t eeprom_readStatus(rtc_eeprom_t *rtc_eeprom);

void eeprom_print(rtc_eeprom_t *rtc_eeprom);
void eeprom_printProtected(rtc_eeprom_t *rtc_eeprom);

uint8_t rtc_readSeconds(rtc_eeprom_t* rtc_eeprom);
void rtc_writeSeconds(rtc_eeprom_t* rtc_eeprom, uint8_t val);

void rtc_incrementeSeconds(rtc_eeprom_t* rtc_eeprom);

void rtc_writeMinutes(rtc_eeprom_t* rtc_eeprom, uint8_t val);
uint8_t rtc_readMinutes(rtc_eeprom_t* rtc_eeprom);

void rtc_writeHours(rtc_eeprom_t* rtc_eeprom, uint8_t val);
uint8_t rtc_readHours(rtc_eeprom_t* rtc_eeprom);

void rtc_writeDate(rtc_eeprom_t* rtc_eeprom, uint8_t val);
uint8_t rtc_readDate(rtc_eeprom_t* rtc_eeprom);

void rtc_writeMonth(rtc_eeprom_t* rtc_eeprom, uint8_t val);
uint8_t rtc_readMonth(rtc_eeprom_t* rtc_eeprom);

void rtc_writeYear(rtc_eeprom_t* rtc_eeprom, uint8_t val);
uint8_t rtc_readYear(rtc_eeprom_t* rtc_eeprom);

uint8_t rtc_isPwrFail(rtc_eeprom_t* rtc_eeprom);

uint8_t rtc_isOscRunning(rtc_eeprom_t* rtc_eeprom);

uint8_t rtc_isVbatEnabled(rtc_eeprom);


void rtc_startClock(rtc_eeprom_t* rtc_eeprom);
void rtc_stopClock(rtc_eeprom_t* rtc_eeprom);

void rtc_eeprom_closeAndFree(rtc_eeprom_t *rtc_eeprom);

