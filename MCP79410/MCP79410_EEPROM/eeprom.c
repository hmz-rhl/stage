#include "eeprom.h"

eeprom_t *eeprom_init(void){

    eeprom_t *eeprom = (eeprom_t*)malloc(sizeof(eeprom_t));

    if(eeprom == NULL){

        fprintf(stderr, "%s: eeprom is NULL: %s\n", __func__, strerror(errno));
        exit(EXIT_FAILURE);
    }
    eeprom->fd = open("/dev/i2c-1", O_RDWR);

    if(eeprom->fd < 0) {

        // on retente après 1 seconde si jamais un bug
        sleep(1);
        eeprom->fd = open("/dev/i2c-1", O_RDWR);
        if(eeprom->fd < 0) {
        
            fprintf(stderr, "fonction %s: Unable to open i2c device: %s\n", __func__, strerror(errno));
            exit(EXIT_FAILURE);
        }
        
    }

    printf("%s: i2c device opened successfully\n",__func__);

    if(ioctl(eeprom->fd,I2C_SLAVE,EEPROM_ADDRESS) < 0) {
        printf("ERREUR de setting de la communication avec l'eeprom (0x57) sur i2c\n");
        close(eeprom->fd);
        exit(EXIT_FAILURE);
    }

    printf("%s: i2c communication with EEPROM(0x57) was set successfully\n",__func__);

}

uint8_t eeprom_read(eeprom_t* eeprom, uint8_t reg){


    eeprom->buf[0] = reg;

    if(write(eeprom->fd,eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n", reg, __func__, strerror(errno));

        close(eeprom->fd);
        exit(EXIT_FAILURE);
    }
    

    if(read(eeprom->fd,eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
        close(eeprom->fd);
        exit(EXIT_FAILURE);
    }
    

    usleep(5000);
    return eeprom->buf[0];

}
void eeprom_write(eeprom_t* eeprom, uint8_t reg, uint8_t val){


    if(eeprom == NULL){

        printf("Error %s: eeprom est NULL\n");
        return;
    }

    if(reg>=0x00 && reg<=0x7F){

        eeprom->buf[0] = reg;
        eeprom->buf[1] = val;
    
        printf("on écrit %02X sur 0x00\n", eeprom->buf[1]);
        if(write(eeprom->fd,eeprom->buf,2) != 2){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n", reg, __func__, strerror(errno));

            close(eeprom->fd);
            exit(EXIT_FAILURE);
        }
    //il faut attendre au moins 5ms
        usleep(5000);
    }
    else if(reg>=0xF0 && reg<=0xF7){

        printf("Error %s: vous avez choisi un registre protege (0xF0 - 0xF7), utiliser plutot : eeprom_writeProtected()\n", reg);

    }
    else{

        printf("Error %s: vous n'avez pas choisi un registre valide (0x00 - 0x7F), vous avez choisi : %02X\n", reg);
        
    }

    return;
}

uint8_t eeprom_readProtected(eeprom_t* eeprom, uint8_t reg){
    
    return 0;
}
void eeprom_writeProtected(eeprom_t* eeprom, uint8_t reg, uint8_t val){

}

void eeprom_closeAndFree(eeprom_t* eeprom){

    close(eeprom->fd);
    free(eeprom);
}
