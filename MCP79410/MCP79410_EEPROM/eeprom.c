#include "eeprom.h"

eeprom_t *eeprom_init(void){

    eeprom_t *eeprom = (eeprom_t*)malloc(sizeof(eeprom_t));

    if(eeprom == NULL){

        fprintf(stderr, "%s: eeprom is NULL: %s\n", __func__, strerror(errno));
        exit(EXIT_FAILURE);
    }
    eeprom->eeprom_fd = open("/dev/i2c-1", O_RDWR);
    eeprom->rtc_eeprom_fd = open("/dev/i2c-1", O_RDWR);

    if(eeprom->eeprom_fd < 0) {

        // on retente après 1 seconde si jamais un bug
        sleep(1);
        eeprom->eeprom_fd = open("/dev/i2c-1", O_RDWR);
        if(eeprom->eeprom_fd < 0) {
        
            fprintf(stderr, "fonction %s: Unable to open i2c device: %s\n", __func__, strerror(errno));
            exit(EXIT_FAILURE);
        }
        
    }
    if(eeprom->rtc_eeprom_fd < 0) {

        // on retente après 1 seconde si jamais un bug
        sleep(1);
        eeprom->rtc_eeprom_fd = open("/dev/i2c-1", O_RDWR);
        if(eeprom->rtc_eeprom_fd < 0) {
        
            fprintf(stderr, "fonction %s: Unable to open i2c device: %s\n", __func__, strerror(errno));
            exit(EXIT_FAILURE);
        }
        
    }

    printf("%s: i2c device opened successfully\n",__func__);

    if(ioctl(eeprom->eeprom_fd,I2C_SLAVE,EEPROM_ADDRESS) < 0) {
        printf("ERREUR de setting de la communication avec l'eeprom (0x57) sur i2c\n");
        close(eeprom->eeprom_fd);
        exit(EXIT_FAILURE);
    }
    if(ioctl(eeprom->rtc_eeprom_fd,I2C_SLAVE,RTC_ADDRESS) < 0) {
        printf("ERREUR de setting de la communication avec l'eeprom (0x6F) sur i2c\n");
        close(eeprom->rtc_eeprom_fd);
        exit(EXIT_FAILURE);
    }

    printf("%s: i2c communication with EEPROM(0x57) was set successfully\n",__func__);
    
    return eeprom;

}

uint8_t eeprom_read(eeprom_t* eeprom, uint8_t reg){


    if(eeprom == NULL){

        printf("Error %s: eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }
    if(reg>=0x00 && reg<=0x7F){

        eeprom->buf[0] = reg;
        if(write(eeprom->eeprom_fd,eeprom->buf,1) != 1){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n",  __func__, reg, strerror(errno));

            eeprom_closeAndFree(eeprom);
            exit(EXIT_FAILURE);
        }
        
        
        usleep(100);
        if(read(eeprom->eeprom_fd,eeprom->buf,1) != 1){

            fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
            eeprom_closeAndFree(eeprom);
            exit(EXIT_FAILURE);
        }
        

        usleep(4000);
        return eeprom->buf[0];

    }
    else if(reg>=0xF0 && reg<=0xF7){

        printf("Error %s: vous avez choisi un registre protege (0xF0 - 0xF7), vous avez choisi : %02X utiliser plutot : eeprom_readProtected()\n", __func__, reg);
        eeprom_closeAndFree(eeprom);
        exit(EXIT_FAILURE);
    }
    else{

        printf("Error %s: vous n'avez pas choisi un registre valide (0x00 - 0x7F), vous avez choisi : %02X\n", __func__, reg);
        eeprom_closeAndFree(eeprom);
        exit(EXIT_FAILURE);
    }

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
        if(write(eeprom->eeprom_fd,eeprom->buf,2) != 2){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n", __func__, reg, strerror(errno));

            eeprom_closeAndFree(eeprom);
            exit(EXIT_FAILURE);
        }
    //il faut attendre au moins 5ms
        usleep(4000);
    }
    else if(reg>=0xF0 && reg<=0xF7){

        printf("Error %s: vous avez choisi un registre protege (0xF0 - 0xF7), vous avez choisi : %02X utiliser plutot : eeprom_readProtected()\n", __func__, reg);

    }
    else{

        printf("Error %s: vous n'avez pas choisi un registre valide (0x00 - 0x7F), vous avez choisi : %02X\n", __func__, reg);
        
    }

}

uint8_t eeprom_readProtected(eeprom_t* eeprom, uint8_t reg){
    
    if(eeprom == NULL){

        printf("Error %s: eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }
    if(reg>=0x00 && reg<=0x7F){


        printf("Error %s: vous avez choisi un registre normal (0x00 - 0x7F), vous avez choisi : %02X utiliser plutot : eeprom_read()\n", __func__, reg);
        eeprom_closeAndFree(eeprom);
        exit(EXIT_FAILURE);

    }
    else if(reg>=0xF0 && reg<=0xF7){

        eeprom->buf[0] = reg;
        if(write(eeprom->eeprom_fd,eeprom->buf,1) != 1){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n",  __func__, reg, strerror(errno));

            eeprom_closeAndFree(eeprom);
            exit(EXIT_FAILURE);
        }
        
        
        usleep(100);
        if(read(eeprom->eeprom_fd,eeprom->buf,8) != 8){

            fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
            eeprom_closeAndFree(eeprom);
            exit(EXIT_FAILURE);
        }
        

        usleep(4000);
        return eeprom->buf[0];
        
    }
    else{

        printf("Error %s: vous n'avez pas choisi un registre valide (0x00 - 0x7F), vous avez choisi : %02X\n", __func__, reg);
        eeprom_closeAndFree(eeprom);
        exit(EXIT_FAILURE);
    }
}

void eeprom_writeProtected(eeprom_t* eeprom, uint8_t reg, uint8_t val){

    if(eeprom == NULL){

        printf("Error %s: eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }
    if(reg>=0x00 && reg<=0x7F){


        printf("Error %s: vous avez choisi un registre normal (0x00 - 0x7F), vous avez choisi : %02X utiliser plutot : eeprom_read()\n", __func__, reg);
        eeprom_closeAndFree(eeprom);
        exit(EXIT_FAILURE);

    }
    else if(reg>=0xF0 && reg<=0xF7){

        eeprom->buf[0] = 0x09;
        eeprom->buf[1] = 0x55;
        if(write(eeprom->rtc_eeprom_fd,eeprom->buf,2) != 2){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n", __func__, reg, strerror(errno));

            eeprom_closeAndFree(eeprom);
            exit(EXIT_FAILURE);
        }
        usleep(4000);
        eeprom->buf[0] = 0x09;
        eeprom->buf[1] = 0xAA;
        if(write(eeprom->rtc_eeprom_fd,eeprom->buf,2) != 2){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n",  __func__, reg, strerror(errno));

            eeprom_closeAndFree(eeprom);
            exit(EXIT_FAILURE);
        }

        usleep(4000);
        eeprom->buf[0] = reg;
        eeprom->buf[1] = val;
        if(write(eeprom->eeprom_fd,eeprom->buf,2) != 2){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n",  __func__, reg, strerror(errno));

            eeprom_closeAndFree(eeprom);
            exit(EXIT_FAILURE);
        }
        
        usleep(4000);
        
    }
    else{

        printf("Error %s: vous n'avez pas choisi un registre valide (0x00 - 0x7F), vous avez choisi : %02X\n", __func__, reg);
        eeprom_closeAndFree(eeprom);
        exit(EXIT_FAILURE);
    }
}


uint8_t eeprom_readStatus(eeprom_t* eeprom){

    if(eeprom == NULL){

        printf("Error %s: eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }
    eeprom->buf[0] = 0xFF;
    if(write(eeprom->eeprom_fd,eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur d'écriture(write()) de 0xFF: %s\n", __func__, strerror(errno));

        eeprom_closeAndFree(eeprom);
        exit(EXIT_FAILURE);
    }
    
    

    if(read(eeprom->eeprom_fd,eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
        eeprom_closeAndFree(eeprom);
        exit(EXIT_FAILURE);
    }
    

    usleep(5000);
    return eeprom->buf[0];
    
}


void eeprom_print(eeprom_t *eeprom){
    if(eeprom == NULL){

        printf("Error %s: eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }


    eeprom->buf[0] = 0x00;
    if(write(eeprom->eeprom_fd,eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur d'écriture(write()) de 0x00: %s\n",  __func__, strerror(errno));

        eeprom_closeAndFree(eeprom);
        exit(EXIT_FAILURE);
    }
    
    
    usleep(100);
    if(read(eeprom->eeprom_fd,eeprom->buf,128) != 128){

        fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
        eeprom_closeAndFree(eeprom);
        exit(EXIT_FAILURE);
    }
    
    printf("    EEPROM Registers    \n")
    printf("________________________\n");
    for (size_t i = 0; i < 128; i++)
    {
        /* code */
        printf("| 0x%02X : \t%02X \t|\n",i, eeprom->buf[i]);
    }
    printf("|______________________|\n");
    
        usleep(4000);

}

void eeprom_closeAndFree(eeprom_t* eeprom){

    close(eeprom->eeprom_fd);
    free(eeprom);
}

