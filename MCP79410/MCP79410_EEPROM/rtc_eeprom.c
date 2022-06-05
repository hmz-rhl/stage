/**
 * @file EEPROM.c
 * @author Hamza RAHAL
 * @brief driver pour piloter MCP79410
 * @version 0.1
 * @date 2022-06-04
 * 
 * @copyright Saemload (c) 2022
 * 
 */

#include "rtc_eeprom.h"


/**
 ** 
 * @brief   ouvre et configure l'interface i2c de la RP, instancie une variable de type rtc_eeprom_t
 * 
 *  
 * @return  renvoi un pointeur sur la variable instanciée
 *  
 **/
rtc_eeprom_t *rtc_eeprom_init(void){

    rtc_eeprom_t *rtc_eeprom = (rtc_eeprom_t*)malloc(sizeof(rtc_eeprom_t));

    if(rtc_eeprom == NULL){

        fprintf(stderr, "%s: rtc_eeprom is NULL: %s\n", __func__, strerror(errno));
        exit(EXIT_FAILURE);
    }
    rtc_eeprom->eeprom_fd = open("/dev/i2c-1", O_RDWR);
    rtc_eeprom->rtc_fd = open("/dev/i2c-1", O_RDWR);

    if(rtc_eeprom->eeprom_fd < 0) {

        // on retente après 1 seconde si jamais un bug
        sleep(1);
        rtc_eeprom->eeprom_fd = open("/dev/i2c-1", O_RDWR);
        if(rtc_eeprom->eeprom_fd < 0) {
        
            fprintf(stderr, "fonction %s: Unable to open i2c device: %s\n", __func__, strerror(errno));
            exit(EXIT_FAILURE);
        }
        
    }
    if(rtc_eeprom->rtc_fd < 0) {

        // on retente après 1 seconde si jamais un bug
        sleep(1);
        rtc_eeprom->rtc_fd = open("/dev/i2c-1", O_RDWR);
        if(rtc_eeprom->rtc_fd < 0) {
        
            fprintf(stderr, "fonction %s: Unable to open i2c device: %s\n", __func__, strerror(errno));
            exit(EXIT_FAILURE);
        }
        
    }

    printf("%s: i2c device opened successfully\n",__func__);

    if(ioctl(rtc_eeprom->eeprom_fd,I2C_SLAVE,EEPROM_ADDRESS) < 0) {
        printf("ERREUR de setting de la communication avec l'rtc_eeprom (0x57) sur i2c\n");
        close(rtc_eeprom->eeprom_fd);
        exit(EXIT_FAILURE);
    }
    if(ioctl(rtc_eeprom->rtc_fd,I2C_SLAVE,RTC_ADDRESS) < 0) {
        printf("ERREUR de setting de la communication avec l'rtc_eeprom (0x6F) sur i2c\n");
        close(rtc_eeprom->rtc_fd);
        exit(EXIT_FAILURE);
    }

    printf("%s: i2c communication with EEPROM(0x57) was set successfully\n",__func__);

    usleep(100);
    //configuration
    rtc_eeprom->buf[0] = 0x07;
    rtc_eeprom->buf[1] = 0x08;

    printf("%s on écrit %02X sur 0x07\n",__func__, rtc_eeprom->buf[1]);
    if(write(rtc_eeprom->rtc_fd,rtc_eeprom->buf,2) != 2){

        fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n", __func__, 0x88, strerror(errno));

        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
//il faut attendre au moins 5ms
    usleep(4000);
    return rtc_eeprom;

}


/**
 ** 
 * @brief   lit le contenue d'un registre non protégée
 * 
 * @param   reg adresse en HEXA du registre à lire
 * 
 * 
 * @return  la valeur lu sur 8 bits
 *  
 **/
uint8_t eeprom_read(rtc_eeprom_t* rtc_eeprom, uint8_t reg){


    if(rtc_eeprom == NULL){

        printf("Error %s: rtc_eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }
    if(reg>=0x00 && reg<=0x7F){

        rtc_eeprom->buf[0] = reg;
        if(write(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,1) != 1){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n",  __func__, reg, strerror(errno));

            rtc_eeprom_closeAndFree(rtc_eeprom);
            exit(EXIT_FAILURE);
        }
        
        
        usleep(100);
        if(read(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,1) != 1){

            fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
            rtc_eeprom_closeAndFree(rtc_eeprom);
            exit(EXIT_FAILURE);
        }
        

        usleep(4000);
        return rtc_eeprom->buf[0];

    }
    else if(reg>=0xF0 && reg<=0xF7){

        printf("Error %s: vous avez choisi un registre protege (0xF0 - 0xF7), vous avez choisi : %02X utiliser plutot : eeprom_readProtected()\n", __func__, reg);
        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
    else{

        printf("Error %s: vous n'avez pas choisi un registre valide (0x00 - 0x7F), vous avez choisi : %02X\n", __func__, reg);
        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }

}

/**
 ** 
 * @brief   ecrit un octet dans un registre non protégée
 * 
 * @param   reg adresse en HEXA du registre à lire
 * @param   val valeur à écrire
 * 
 * 
 *  
 **/
void eeprom_write(rtc_eeprom_t* rtc_eeprom, uint8_t reg, uint8_t val){


    if(rtc_eeprom == NULL){

        printf("Error %s: rtc_eeprom est NULL\n");
        return;
    }

    if(reg>=0x00 && reg<=0x7F){

        rtc_eeprom->buf[0] = reg;
        rtc_eeprom->buf[1] = val;
    
        printf("%s on écrit %02X sur 0x00\n",__func__, rtc_eeprom->buf[1]);
        if(write(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,2) != 2){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n", __func__, reg, strerror(errno));

            rtc_eeprom_closeAndFree(rtc_eeprom);
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

/**
 ** 
 * @brief   lit le contenue d'un registre protégée
 * 
 * @param   reg adresse en HEXA du registre à lire
 * 
 * 
 * @return  la valeur lu sur 8 bits
 *  
 **/
uint8_t eeprom_readProtected(rtc_eeprom_t* rtc_eeprom, uint8_t reg){
    
    if(rtc_eeprom == NULL){

        printf("Error %s: rtc_eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }
    if(reg>=0x00 && reg<=0x7F){


        printf("Error %s: vous avez choisi un registre normal (0x00 - 0x7F), vous avez choisi : %02X utiliser plutot : eeprom_read()\n", __func__, reg);
        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);

    }
    else if(reg>=0xF0 && reg<=0xF7){

        rtc_eeprom->buf[0] = reg;
        if(write(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,1) != 1){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n",  __func__, reg, strerror(errno));

            rtc_eeprom_closeAndFree(rtc_eeprom);
            exit(EXIT_FAILURE);
        }
        
        
        usleep(100);
        if(read(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,8) != 8){

            fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
            rtc_eeprom_closeAndFree(rtc_eeprom);
            exit(EXIT_FAILURE);
        }
        

        usleep(4000);
        return rtc_eeprom->buf[0];
        
    }
    else{

        printf("Error %s: vous n'avez pas choisi un registre valide (0x00 - 0x7F), vous avez choisi : %02X\n", __func__, reg);
        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
}

/**
 ** 
 * @brief   ecrit un octet dans un registre protégée
 * 
 * @param   reg adresse en HEXA du registre à lire
 * @param   val valeur à écrire
 * 
 * 
 *  
 **/
void eeprom_writeProtected(rtc_eeprom_t* rtc_eeprom, uint8_t reg, uint8_t val){

    if(rtc_eeprom == NULL){

        printf("Error %s: rtc_eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }
    if(reg>=0x00 && reg<=0x7F){


        printf("Error %s: vous avez choisi un registre normal (0x00 - 0x7F), vous avez choisi : %02X utiliser plutot : eeprom_read()\n", __func__, reg);
        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);

    }
    else if(reg>=0xF0 && reg<=0xF7){

        rtc_eeprom->buf[0] = 0x09;
        rtc_eeprom->buf[1] = 0x55;
        if(write(rtc_eeprom->rtc_fd,rtc_eeprom->buf,2) != 2){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n", __func__, reg, strerror(errno));

            rtc_eeprom_closeAndFree(rtc_eeprom);
            exit(EXIT_FAILURE);
        }
        usleep(4000);
        rtc_eeprom->buf[0] = 0x09;
        rtc_eeprom->buf[1] = 0xAA;
        if(write(rtc_eeprom->rtc_fd,rtc_eeprom->buf,2) != 2){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n",  __func__, reg, strerror(errno));

            rtc_eeprom_closeAndFree(rtc_eeprom);
            exit(EXIT_FAILURE);
        }

        usleep(4000);
        rtc_eeprom->buf[0] = reg;
        rtc_eeprom->buf[1] = val;
        if(write(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,2) != 2){

            fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n",  __func__, reg, strerror(errno));

            rtc_eeprom_closeAndFree(rtc_eeprom);
            exit(EXIT_FAILURE);
        }
        
        usleep(4000);
        
    }
    else{

        printf("Error %s: vous n'avez pas choisi un registre valide (0x00 - 0x7F), vous avez choisi : %02X\n", __func__, reg);
        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
}

/**
 ** 
 * @brief   lit le contenue du registre STATUS(0xFF)
 * 
 * 
 * @return  la valeur de STATUS lu sur 8 bits
 *  
 **/
uint8_t eeprom_readStatus(rtc_eeprom_t* rtc_eeprom){

    if(rtc_eeprom == NULL){

        printf("Error %s: rtc_eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }
    rtc_eeprom->buf[0] = 0xFF;
    if(write(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur d'écriture(write()) de 0xFF: %s\n", __func__, strerror(errno));

        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
    
    

    if(read(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
    

    usleep(5000);
    return rtc_eeprom->buf[0];
    
}


/**
 ** 
 * @brief   Affiche le contenue de l'eeprom non protégé sur la console
 * 
 *  
 **/
void eeprom_print(rtc_eeprom_t *rtc_eeprom){
    if(rtc_eeprom == NULL){

        printf("Error %s: rtc_eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }


    rtc_eeprom->buf[0] = 0x00;
    if(write(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur d'écriture(write()) de 0x00: %s\n",  __func__, strerror(errno));

        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
    
    
    usleep(100);
    if(read(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,128) != 128){

        fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
    
    printf("    EEPROM Registers    \n");
    printf("________________________\n");
    for (size_t i = 0; i < 128; i++)
    {
        /* code */
        printf("| 0x%02X : \t%02X \t|\n",i, rtc_eeprom->buf[i]);
    }
    printf("|_______________________|\n\n");
    
        usleep(4000);

}

/**
 ** 
 * @brief   Affiche le contenue de l'eeprom protégée
 *
 *  
 **/
void eeprom_printProtected(rtc_eeprom_t *rtc_eeprom){

    if(rtc_eeprom == NULL){

        printf("Error %s: rtc_eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }


    rtc_eeprom->buf[0] = 0xF0;
    if(write(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur d'écriture(write()) de 0x00: %s\n",  __func__, strerror(errno));

        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
    
    
    usleep(100);
    if(read(rtc_eeprom->eeprom_fd,rtc_eeprom->buf,8) != 8){

        fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
    
    printf("EEPROM Protected Registers\n");
    printf("__________________________\n");
    for (size_t i = 0; i < 8; i++)
    {
        /* code */
        printf("| 0x%02X : \t%02X \t  |\n",i+0XF0, rtc_eeprom->buf[i]);
    }
    printf("|_________________________|\n\n");
    
        usleep(4000);

}


uint8_t rtc_readSeconds(rtc_eeprom_t* rtc_eeprom){

    if(rtc_eeprom == NULL){

        printf("Error %s: rtc_eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }

    rtc_eeprom->buf[0] = 0x00;
    if(write(rtc_eeprom->rtc_fd,rtc_eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur d'écriture(write()) de 0x00: %s\n",  __func__, strerror(errno));

        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
    
    
    usleep(100);
    if(read(rtc_eeprom->rtc_fd,rtc_eeprom->buf,1) != 1){

        fprintf(stderr, "fonction %s: erreur de lecture(read()): %s\n", __func__, strerror(errno));
        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
    usleep(4000);
    
    return rtc_eeprom->buf[0];
}

void rtc_writeSeconds(rtc_eeprom_t* rtc_eeprom, uint8_t val){
    
    if(rtc_eeprom == NULL){

        printf("Error %s: rtc_eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }

    rtc_eeprom->buf[0] = 0x00;
    rtc_eeprom->buf[1] = val | (rtc_readSeconds(rtc_eeprom) & 0x80);

    printf("%s on écrit %02X sur 0x00\n",__func__, rtc_eeprom->buf[1]);
    if(write(rtc_eeprom->rtc_fd,rtc_eeprom->buf,2) != 2){

        fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n", __func__, 0, strerror(errno));

        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
//il faut attendre au moins 5ms
    usleep(4000);
    

}


void rtc_startClock(rtc_eeprom_t* rtc_eeprom){
    
    if(rtc_eeprom == NULL){

        printf("Error %s: rtc_eeprom est NULL\n");
        exit(EXIT_FAILURE);
    }

    rtc_eeprom->buf[0] = 0x00;
    rtc_eeprom->buf[1] = 0x80 | (rtc_readSeconds(rtc_eeprom) );

    printf("%s on écrit %02X sur 0x00\n",__func__, rtc_eeprom->buf[1]);
    if(write(rtc_eeprom->rtc_fd,rtc_eeprom->buf,2) != 2){

        fprintf(stderr, "fonction %s: erreur d'écriture(write()) de %02X: %s\n", __func__, 0, strerror(errno));

        rtc_eeprom_closeAndFree(rtc_eeprom);
        exit(EXIT_FAILURE);
    }
//il faut attendre au moins 5ms
    usleep(4000);
    

}




/**
 ** 
 * @brief   libère les descripteurs ainsi que la mémoire allouée
 * 
 * 
 *  
 **/

void rtc_eeprom_closeAndFree(rtc_eeprom_t* rtc_eeprom){

    close(rtc_eeprom->eeprom_fd);
    close(rtc_eeprom->rtc_fd);
    free(rtc_eeprom);
}

