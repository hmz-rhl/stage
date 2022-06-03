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

#define I2C_DEVICE "/dev/i2c-1"
#define EEUNLOCK 0x09
#define EEPROM_ADDRESS 0x57

int main(int argc, char const *argv[])
{
    /* code */

    int fd = open(I2C_DEVICE, O_RDWR);
    if(fd < 0) {

        sleep(1);
        fd = open(I2C_DEVICE, O_RDWR);
        if(fd < 0) {
        
            fprintf(stderr, "fonction %s: Unable to open i2c device: %s\n", __func__, strerror(errno));
            exit(EXIT_FAILURE);
        }
        
    }

    if(ioctl(fd,I2C_SLAVE,EEPROM_ADDRESS) < 0) {
        printf("ERREUR de setting de la communication avec 0x57 sur i2c\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    uint8_t buf[10];
// lecture
    buf[0] = 0xF0;
    if(write(fd,buf,1) != 1){

        printf("erreur de selectioj du registre F0\n");
        exit(EXIT_FAILURE);
    }
    
    if(read(fd,buf,1) != 1){

        printf("erreur de lecture de F0\n");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);

// ecriture
    buf[0] = EEUNLOCK;
    buf[1] = 0x55;

    if(write(fd,buf,2) != 2){

        printf("erreur d'écriture de 0x55 dans EEUNLOCK\n");
        exit(EXIT_FAILURE);
    }

    buf[0] = EEUNLOCK;
    buf[1] = 0xAA;
    if(write(fd,buf,2) != 2){

        printf("erreur d'écriture de 0xAA dans EEUNLOCK\n");
        exit(EXIT_FAILURE);
    }

    buf[0] = 0xF0;
    buf[1] = 'a';
 

    if(write(fd,buf,2) != 2){

        printf("erreur d'écriture de aAbBcCdD dans F0\n");
        exit(EXIT_FAILURE);
    }

// lecture
    buf[0] = 0xF0;
    if(write(fd,buf,1) != 1){

        printf("erreur de selectioj du registre F0\n");
        exit(EXIT_FAILURE);
    }
    
    if(read(fd,buf,1) != 1){

        printf("erreur de lecture de F0\n");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);


    
    
    return 0;
}
