/**
 * @file expander-set-gpio.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.2
 * @date 2022-05-27
 * 
 * @copyright Copyright (c) 2022
 * 
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
#include <expander_i2c.h>

#define I2C_DEVICE          "/dev/i2c-1"
#define MCP23008_ADDR       (0x26)


int fd;
uint8_t buff = 0;;

//expander-set-gpio 26 --pin 0-7 0-1

int main(int argc, char* argv[]) {



    if(argc == 2 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help"))){

        printf("Usage: ./expander-set-gpio -h, --help \taffiche ce message d'aide \n");
        printf("       ./expander-set-gpio <addresse> <GPIO 7> ... <GPIO 0> \n");


        printf("exemple: ./expander-set-gpio 27 0 1 0 1 0 1 1 1\n");
        exit(EXIT_SUCCESS);

    }

    if(argc != 10 || argc != 4){

        printf("Usage:   ./expander-set-gpio <addresse> <GPIO 7> ... <GPIO 0> \n");
        printf("exemple: ./expander-set-gpio 26 0 1 0 1 0 1 1 1\n");
        exit(EXIT_FAILURE);

    }

    if(!strcmp(argv[1], "27")){
        expander_t *exp = expander_init(0x27);
        
        if(argc == 10)
        {
            for (size_t i = 0; i < 8; i++)
            {
                buff += (uint8_t)(pow(2,i)*(atoi(argv[9-i])));
            }
            expander_setAndResetSomePinsGPIO(exp, buff);
            expander_printGPIO(exp);
        }
        if(argc == 4)
        {
            if(((!strcmp(argv[2], "0") || !strcmp(argv[2], "1") || !strcmp(argv[2], "3") || !strcmp(argv[2], "4") || !strcmp(argv[2], "5") || !strcmp(argv[2], "6") || !strcmp(argv[2], "7")) && !strcmp(argv[3], "1")))
            {
    
                expander_setPinGPIO(exp,atoi(argv[2]));
            }
            else if(((!strcmp(argv[2], "0") || !strcmp(argv[2], "1") || !strcmp(argv[2], "3") || !strcmp(argv[2], "4") || !strcmp(argv[2], "5") || !strcmp(argv[2], "6") || !strcmp(argv[2], "7")) && !strcmp(argv[3], "0")))
            {
                expander_resetPinGPIO(exp,atoi(argv[2]));
            }
        }
        expander_closeAndFree(exp);
    }

    

       

    

   


    if(!strcmp(argv[1], "26")){
        
        expander_t *exp = expander_init(0x26);
        if(argc == 10)
        {
            for (size_t i = 0; i < 8; i++)
            {
                buff += (uint8_t)(pow(2,i)*(atoi(argv[9-i])));
            }
            expander_setAndResetSomePinsGPIO(exp, buff);
            printf("%02x\n",buff);
            expander_printGPIO(exp);
        }
        if(argc == 4)
        {
            if(((!strcmp(argv[2], "0") || !strcmp(argv[2], "1") || !strcmp(argv[2], "3") || !strcmp(argv[2], "4") || !strcmp(argv[2], "5") || !strcmp(argv[2], "6") || !strcmp(argv[2], "7")) && !strcmp(argv[3], "1")))
            {
    
                expander_setPinGPIO(exp,atoi(argv[2]));
            }
            else if(((!strcmp(argv[2], "0") || !strcmp(argv[2], "1") || !strcmp(argv[2], "3") || !strcmp(argv[2], "4") || !strcmp(argv[2], "5") || !strcmp(argv[2], "6") || !strcmp(argv[2], "7")) && !strcmp(argv[3], "0")))
            {
                expander_resetPinGPIO(exp,atoi(argv[2]));
            }
        }
        expander_closeAndFree(exp);

    }
    
        else if(!strcmp(argv[1], "25")){

        expander_t *exp = expander_init(0x25);
        for (size_t i = 0; i < 8; i++)
        {
            buff += (uint8_t)(pow(2,i)*(atoi(argv[9-i])));
        }
        expander_setAndResetSomePinsGPIO(exp, buff);
        expander_printGPIO(exp);
        expander_closeAndFree(exp);

    }
    else if(!strcmp(argv[1], "24")){

        expander_t *exp = expander_init(0x24);
        for (size_t i = 0; i < 8; i++)
        {
            buff += (uint8_t)(pow(2,i)*(atoi(argv[9-i])));
        }
        expander_setAndResetSomePinsGPIO(exp, buff);
        expander_printGPIO(exp);
        expander_closeAndFree(exp);

    }
        else if(!strcmp(argv[1], "23")){

        expander_t *exp = expander_init(0x23);
        for (size_t i = 0; i < 8; i++)
        {
            buff += (uint8_t)(pow(2,i)*(atoi(argv[9-i])));
        }
        expander_setAndResetSomePinsGPIO(exp, buff);
        expander_printGPIO(exp);
        expander_closeAndFree(exp);

    }
            else if(!strcmp(argv[1], "22")){

        expander_t *exp = expander_init(0x22);
        for (size_t i = 0; i < 8; i++)
        {
            buff += (uint8_t)(pow(2,i)*(atoi(argv[9-i])));
        }
        expander_setAndResetSomePinsGPIO(exp, buff);
        expander_printGPIO(exp);
        expander_closeAndFree(exp);

    }
            else if(!strcmp(argv[1], "21")){

        expander_t *exp = expander_init(0x21);
        for (size_t i = 0; i < 8; i++)
        {
            buff += (uint8_t)(pow(2,i)*(atoi(argv[9-i])));
        }
        expander_setAndResetSomePinsGPIO(exp, buff);
        expander_printGPIO(exp);
        expander_closeAndFree(exp);

    }
            else if(!strcmp(argv[1], "20")){

        expander_t *exp = expander_init(0x20);
        for (size_t i = 0; i < 8; i++)
        {
            buff += (uint8_t)(pow(2,i)*(atoi(argv[9-i])));
        }
        expander_setAndResetSomePinsGPIO(exp, buff);
        expander_printGPIO(exp);
        expander_closeAndFree(exp);

    }




    exit(EXIT_SUCCESS);



    

}

