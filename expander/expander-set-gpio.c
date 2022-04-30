/*

    __________Fonctionnel !___________
    
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include "../lib/expander-i2c.h"

#define I2C_DEVICE          "/dev/i2c-1"
#define MCP23008_ADDR       (0x26)

/**
 * 
 * a corgier le dynamisme de l'adresse
 * 
 **/


// registers
#define REG_IODIR   0x00   //!< I/O direction register
#define REG_IPOL    0x01    //!< Input polarity register
#define REG_GPINTEN 0x02 //!< Interrupt-on-change control register
#define REG_DEFVAL  0x03 //!< Default compare register for interrupt-on-change
#define REG_INTCON  0x04 //!< Interrupt control register
#define REG_IOCON   0x05  //!< Configuration register
#define REG_GPPU    0x06   //!< Pull-up resistor configuration register
#define REG_INTF    0x07   //!< Interrupt flag register
#define REG_INTCAP  0x08 //!< Interrupt capture register
#define REG_GPIO    0x09   //!< Port register
#define REG_OLAT    0x0A   //!< Output latch register

#define VERSION "1.1"

int fd;
int ret;
uint8_t buff = 0;;



int main(int argc, char* argv[]) {


    if(argc != 10){
        printf("Usage:   ./expander-get-gpio <addr> <GPIO 7> ... <GPIO 0> \n");
        printf("exemple: ./expander-get-gpio 26 0 1 0 1 0 1 1 1\n");
        exit(EXIT_FAILURE);

    }
    if(argc == 2 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help"))){
        printf("Usage: ./expander-get-gpio -h, --help affiche ce message d'aide \n");
        printf("       ./expander-get-gpio <GPIO 7> ... <GPIO 0> \n");

        printf("exemple: ./expander-get-gpio 0 1 0 1 0 1 1 1\n");
        exit(EXIT_FAILURE);

    }
    if(argc == 2 && (!strcmp(argv[1],"-v") || !strcmp(argv[1],"--version"))){
        
        printf("version : %s\n", VERSION);
        putchar('\n');
        exit(EXIT_FAILURE);

    }
    if(!strcmp(argv[0], "26")){
        expander_t exp = expander_init(0x26);
    }
    else if(!strcmp(argv[0], "27")){
        expander_t exp = expander_init(0x27);
    }
    for (size_t i = 0; i < 8; i++)
    {
       buff += (uint8_t)(pow(2,i)*(atoi(argv[9-i])));
    }

    expander_setAndResetSomePinsGPIO(exp, buff);


    expander_closeAndFree(exp);
    close(fd);

    exit(EXIT_SUCCESS);



    

}