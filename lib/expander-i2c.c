/**
 * @file expander-i2c.c
 * @author Hamza RAHAL
 * @brief 
 * @version 0.1
 * @date 2022-05-14
 * 
 * @copyright Saemload (c) 2022
 * 
 */


#include "expander-i2c.h"


/**
 ** 
 * @brief   ouvre et configure l'interface i2c de la RP, instancie une variable de type expander_t et initialise ses champs dont l'adresse esclave du MCP
 * 
 * @param   addr adresse en HEXA du MCP23008 (0x__)
 * 
 * 
 * @return  renvoi un pointeur sur la variable instanciée
 *  
 **/
expander_t* expander_init(uint8_t addr){
    if(addr > 0x27 || addr < 0x20 )
    {
        printf("ERREUR %s : vous avez saisie 0x%02x\nOr addr doit etre entre 0x20 et 0x27 pour l'expander\n",__func__);
        exit(EXIT_FAILURE);
    }
    expander_t* exp = malloc(sizeof(expander_t));
    if (exp == NULL){
        printf("ERREUR %s : allocation echouee\n", __func__);
        exit(EXIT_FAILURE);
    }


    exp->addr = addr;
    expander_labelize(exp);
    expander_openI2C(exp);
    expander_setI2C(exp);

    return exp;
}



/**
 ** 
 * @brief   definit le champs label qui associera un nom a chaque pin GPIO pour la console
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 *  
 **/
void expander_labelize(expander_t* exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }
    if(exp->addr == 0x26){

        strcpy(exp->label[0],"TYPE-2_NL1_ON*---->");
        strcpy(exp->label[1],"TYPE-2_L2L3_ON*--->");
        strcpy(exp->label[2],"TYPE-E/F_ON*------>");
        strcpy(exp->label[3],"LOCK_D*----------->");
        strcpy(exp->label[4],"RCD_DIS#*--------->");
        strcpy(exp->label[5],"RCD_TST#*--------->");
        strcpy(exp->label[6],"RCD_RESET#*------->");
        strcpy(exp->label[7],"------------------>");

    }
    else{

        strcpy(exp->label[0],"LED_DIS#*--------->");
        strcpy(exp->label[1],"CP_DIS#*---------->");
        strcpy(exp->label[2],"PP_CS*------------>");
        strcpy(exp->label[3],"CP_CS*------------>");
        strcpy(exp->label[4],"T_CS*------------->");
        strcpy(exp->label[5],"PM_CS*------------>");
        strcpy(exp->label[6],"PM1*-------------->");
        strcpy(exp->label[7],"PM0*-------------->");
    }


}



/**
 ** 
 * @brief   ouvre l'interface i2c de la RP
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 *  
 **/
void expander_openI2C(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }
    exp->fd = open(I2C_DEVICE, O_RDWR);
    if(exp->fd < 0) {

        sleep(1);
        exp->fd = open(I2C_DEVICE, O_RDWR);
        if(exp->fd < 0) {
        
            fprintf(stderr, "fonction %s: Unable to open i2c device: %s\n", __func__, strerror(errno));
            exit(EXIT_FAILURE);
        }
        
    }
}



/**
 ** 
 * @brief   ferme l'interface i2c de la RP
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 *  
 **/
void expander_closeI2C(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }
    if(close(exp->fd) < 0) {

        fprintf(stderr, "fonction %s: Unable to close i2c device: %s\n", __func__, strerror(errno));
        exit(EXIT_FAILURE);
    }
}



/**
 ** 
 * @brief   configure l'interface i2c, et lui fait connaitre l'adresse de l'expander
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 *  
 **/
void expander_setI2C(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }

    if(ioctl(exp->fd,I2C_SLAVE,exp->addr) < 0) {
        printf("ERREUR de setting de l'address l'interface I2C de la RPZ ...\n");
        close(exp->fd);
        exit(EXIT_FAILURE);
    }

}



/**
 ** 
 * @brief   Renvoi l'état des pins GPIO (0-7)
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 *  @return l'état des pins sous forme d'un octet où chaque bit correspond a un pin
 * 
 *  **/
uint8_t expander_getAllPinsGPIO(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }

/**
 * Selection du registre GPIO de l'expoander
 **/
    exp->buff[0] = REG_GPIO; 
    if(write(exp->fd,exp->buff,1) != 1){
        
        printf("ERREUR d'écriture du registre GPIO (branché sur i2c?)\n");
        close(exp->fd);
        exit(EXIT_FAILURE);
    }
/**
 * Lecture du registre GPIO de l'expander
 **/
    if(read(exp->fd,exp->buff,1) != 1) {
        printf("ERREUR de de lecture sur GPIO\n");
        close(exp->fd);
        exit(EXIT_FAILURE);
    }
    
    return exp->buff[0];

}



/**
 * 
 * @brief   Renvoi l'état du pin
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 *  @return 0x00 ou 0x01 en fonction de l'état du pin
 * 
 *  **/
uint8_t expander_getPinGPIO(expander_t *exp, uint8_t pin){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }
        if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin selectionne non compris entre 0 et 7\n", __func__);
        exit(EXIT_FAILURE);
    }

    uint8_t ret = expander_getAllPinsGPIO(exp);

    return (ret >> pin) & 0x01;
}



/**
 * 
 * @brief   mets un pin a 1
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 * 
 *  **/
void expander_setPinGPIO(expander_t *exp, uint8_t pin){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }
    
    if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin doit etre compris entre 0 et 7\n", __func__);
        exit(EXIT_FAILURE);
    }
    uint8_t ancienGPIO = expander_getAllPinsGPIO(exp);
    uint8_t nouveauGPIO = ancienGPIO | (0x01 << pin);

/* Ecriture des gpio de l'expander
 **/

    exp->buff[0] = MCP23008_IODIR;
    exp->buff[1] = 0x00;

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur IODIR\r\n");
        exit(EXIT_FAILURE);
    }
    exp->buff[0] = REG_OLAT;

    exp->buff[1] = nouveauGPIO;

#ifdef DEBUG
    printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
#endif

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur OLAT\r\n");
        exit(EXIT_FAILURE);
    }
    #ifdef DEBUG
    printf("mise a 1 de GPIO[%d] %s\n", pin, exp->label[pin]);
#endif
}



/**
 * 
 * @brief   mets un pin a 0
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 * 
 *  **/
void expander_resetPinGPIO(expander_t *exp, uint8_t pin){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }

    if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin doit etre compris entre 0 et 7\n", __func__);
        exit(EXIT_FAILURE);
    }

    uint8_t ancienGPIO = expander_getAllPinsGPIO(exp);
    uint8_t nouveauGPIO = ancienGPIO & ~(0x01 << pin);

/* Ecriture des gpio de l'expander
 **/
    exp->buff[0] = MCP23008_IODIR;
    exp->buff[1] = 0x00;

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur IODIR\r\n");
        exit(EXIT_FAILURE);
    }

    exp->buff[0] = REG_OLAT;

    exp->buff[1] = nouveauGPIO;

#ifdef DEBUG
    printf("__Ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
#endif

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur OLAT\r\n");
        exit(EXIT_FAILURE);
    }
    #ifdef DEBUG
    printf("mise a 0 de GPIO[%d] %s\n", pin , exp->label[pin]);
#endif

}



/**
 * 
 * @brief   inverse l'état d'un pin
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 * 
 *  **/
void

 expander_togglePinGPIO(expander_t* exp, uint8_t pin){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }

    if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin doit etre compris entre 0 et 7\n", __func__);
        exit(EXIT_FAILURE);
    }

     if(expander_getPinGPIO(exp, pin)){

        expander_resetPinGPIO(exp,pin);
    }
    else{
        expander_setPinGPIO(exp,pin);
    }
}

/**
 ** 
 * @brief   mets tout les pins a 1
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 * 
 *  **/
void expander_setAllPinsGPIO(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }

/* Ecriture des gpio de l'expander
 **/
    exp->buff[0] = MCP23008_IODIR;
    exp->buff[1] = 0x00;

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur IODIR\r\n");
        exit(EXIT_FAILURE);
    }

    exp->buff[0] = REG_OLAT;
    exp->buff[1] = 0xFF;
#ifdef DEBUG
    printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
#endif

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur OLAT\r\n");
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    printf("mise a 1 de tous les GPIO\n");
#endif
}



/**
 * 
 * @brief   mets tout les pin a 0
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 * 
 *  **/
void expander_resetAllPinsGPIO(expander_t *exp){


    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }

/* Ecriture des gpio de l'expander
 **/
    exp->buff[0] = MCP23008_IODIR;
    exp->buff[1] = 0x00;

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur IODIR\r\n");
        exit(EXIT_FAILURE);
    }

    exp->buff[0] = REG_OLAT;
#ifdef DEBUG
    printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
#endif
    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur OLAT\r\n");
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    printf("mise a 0 de tous les GPIO\n");
    #endif
}



/**
 * 
 * @brief   mets le pin a 1 et tout les autres a 0
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 * 
 *  **/
void expander_setOnlyPinResetOthersGPIO(expander_t* exp, uint8_t pin){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }
    
    if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin doit etre compris entre 0 et 7\n", __func__);
        exit(EXIT_FAILURE);
    }

    exp->buff[0] = MCP23008_IODIR;
    exp->buff[1] = 0x00;

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur IODIR\r\n");
        exit(EXIT_FAILURE);
    }
    exp->buff[0] = REG_OLAT;
    exp->buff[1] = 0x01 << pin;
    #ifdef DEBUG
    printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
    #endif
    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur OLAT\r\n");
        exit(EXIT_FAILURE);
    }
    #ifdef DEBUG
    printf("mise a 1 du seul GPIO[%d] %s\n", pin, exp->label[pin]);
    #endif
}



/**
 * 
 * @brief    mets le pin a 0 et tout les autres a 1
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 * 
 *  **/
void expander_resetOnlyPinSetOthersGPIO(expander_t* exp, uint8_t pin){
    
    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }

    if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin doit etre compris entre 0 et 7\n", __func__);
        exit(EXIT_FAILURE);
    }
    exp->buff[0] = MCP23008_IODIR;
    exp->buff[1] = 0x00;

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur IODIR\r\n");
        exit(EXIT_FAILURE);
    }
    
    exp->buff[0] = REG_OLAT;
    exp->buff[1] = ~(0x01 << pin);
#ifdef DEBUG
    printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
#endif
    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur OLAT\r\n");
        exit(EXIT_FAILURE);
    }
    #ifdef DEBUG
    printf("mise a 1 du seul GPIO[%d]\n", pin);
#endif
}


void expander_setAndResetSomePinsGPIO(expander_t* exp, uint8_t config){

        
    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }
        exp->buff[0] = 0x00;
        exp->buff[1] = 0;

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur IODIR\r\n");
        exit(EXIT_FAILURE);
    }

    exp->buff[0] = REG_OLAT;
    exp->buff[1] = config;
#ifdef DEBUG
    printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
#endif

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur OLAT\r\n");
        exit(EXIT_FAILURE);
    }
    #ifdef DEBUG
    printf("mise a %02x du GPIO\n", config);
#endif
}

/**
 * 
 * @brief    Affiche l'etat des pin sur la console
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 * 
 *  **/
void expander_printGPIO(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        exit(EXIT_FAILURE);
    }

/**
 * Selection du registre GPIO de l'expoander
 **/
    exp->buff[0] = REG_GPIO; 
    if(write(exp->fd,exp->buff,1) != 1){
        
        printf("ERREUR d'écriture du registre GPIO (branché sur i2c?)\n");
        close(exp->fd);
        exit(EXIT_FAILURE);
    }
/**
 * Lecture du registre GPIO de l'expander
 **/
    if(read(exp->fd,exp->buff,1) != 1) {
        printf("ERREUR de de lecture sur GPIO\n");
        close(exp->fd);
        exit(EXIT_FAILURE);
    }
/**
 * Affichage des ports GPIO de l'expander
 **/
    printf("___");
    printf("Expander 0x%02x", exp->addr);
    printf("_______________\n");
    for (size_t i = 0; i < 8; i++)
    {
        
        printf("%s GPIO[%d] : %d\r\n",exp->label[i], i, (exp->buff[0] >> i ) & 0x01);
    }
    printf("_______________________________\n");
    putchar('\n');
}



/**
 * 
 * @brief   ferme l'interface i2C et libère la memoire utilisé pour exp
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 * 
 *  **/
void expander_closeAndFree(expander_t *exp)
{
    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        return;
    }
    expander_closeI2C(exp);
    free(exp);
}
