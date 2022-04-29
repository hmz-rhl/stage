#include "../lib/expander-i2c.h"


#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define VERSION_16       0x4FE //Reset: 0x0040 Access: R

#define ADE9078_VERBOSE_DEBUG

#define EXPANDER_1      0x26  // Adresse de l'expander 1 d'Output

#define EXPANDER_2      0x27  // Adresse de l'expander 2 d'Output

#define IRQ1              24

#define RUN             0x480

#define READ            0x0008
#define WRITE            0xFFF6

#define PART_ID         0x472

#define PGA_GAIN        0x4B9  //  0x******* Vc Vb Va In Ic Ib Ia (chaqun sur 2bits (1 2 4 4) donc sur les 14 premiers bits du registre )

#define DEBUG


void waitForReady(expander_t *exp){
	
	time_t start, end;
	double attente = 0;
	start = clock();
	while(( expander_getAllPinsGPIO(exp) & (uint8_t)0b00111100 != 0b00111100 ))
	{
		end = clock();
		attente = (double)(end - start) / (double)(CLOCKS_PER_SEC);
		if(attente > 5)
		{
			perror("Erreur timeout: SPI busy tout les CS ne sont pas a 1");
			exit(EXIT_FAILURE);
		}
	}
}

void setAllCS(expander_t *exp)
{
  expander_setPinGPIO(exp, 2);
  expander_setPinGPIO(exp, 3);
  expander_setPinGPIO(exp, 4);
  expander_setPinGPIO(exp, 5);
}

uint16_t ADE9078_getRun(){
       
	if(wiringPiSPISetup(0, 2000000) < 0)
	{
		perror("Erreur de setup du SPI");
		exit(EXIT_FAILURE);
	}

	uint8_t data[6] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (RUN >> 4) ;
	data[1] = ((RUN & 0x00F) << 4) | READ;
  // data[2] = 0x00;
  // data[3] = 0x01;



  
  	while(digitalRead(IRQ1));
	waitForReady(exp);
	expander_t *exp = expander_init(EXPANDER_2);

	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  // expander_resetAllPinsGPIO(exp);
  	setAllCS(exp);

	// cs de Temperature adc a 0 uniquement lui les autres 1 
	usleep(1);
	
  	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);

#ifdef DEBUG
  	printf("|read run register|\n");
#endif
	wiringPiSPIDataRW(0, data,4);

  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);

	usleep(1);




  uint16_t recu = data[3] + (data[2] << 8);

#ifdef DEBUG
  printf("RUN : %d\n", recu);
#endif

  expander_closeAndFree(exp);

  return recu;
}


void ADE9078_setRun(){
       
	if(wiringPiSPISetup(0, 2000000) < 0)
	{
		perror("Erreur de setup du SPI");
		exit(EXIT_FAILURE);
	}

	uint8_t data[6] = {0};

	

	data[0] = 0x00FF & (RUN >> 4) ;
	data[1] = ((RUN & 0x00F) << 4) & WRITE;
  	data[2] = 0x00;
  	data[3] = 0x01;


#ifdef DEBUG
#endif
// on attend que irq1 passe a 0
 	while(digitalRead(IRQ1));

	expander_t *exp = expander_init(EXPANDER_2);

	// on attend que tout les CS se libere pour eviter d'entrer en conflit sur le bus spi
	// on si on depasse un certain timeout on return
	waitForReady(exp);
	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  // expander_resetAllPinsGPIO(exp);
 	setAllCS(exp);

	usleep(1);
	
	// cs de Temperature ADE a 0 uniquement lui les autres 1 
 	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);
#ifdef DEBUG
 	printf("|write 1 on run register|\n");
#endif
	wiringPiSPIDataRW(0, data,4);

  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);

	usleep(1);

  expander_closeAndFree(exp);

}


void ADE9078_ConfigAPGAIN(){
       
	if(wiringPiSPISetup(0, 2000000) < 0)
	{
		perror("Erreur de setup du SPI");
		exit(EXIT_FAILURE);
	}

	uint8_t data[6] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (RUN >> 4) ;
	data[1] = ((RUN & 0x00F) << 4) & WRITE;
  data[2] = 0x00;
  data[3] = 0x00;



  printf("on envoie : 0x%02X %02X %02X %02X\n\n", data[3], data[2], data[1], data[0]);
  
  	while(digitalRead(IRQ1));

	expander_t *exp = expander_init(EXPANDER_2);
	waitForReady(exp);
	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  // expander_resetAllPinsGPIO(exp);
  	setAllCS(exp);

	// cs de Temperature adc a 0 uniquement lui les autres 1 
	usleep(1);
	
  	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);

	wiringPiSPIDataRW(0, data,4);

  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);

	usleep(1);

  expander_closeAndFree(exp);

}


void ADE9078_resetRun(){
       
	if(wiringPiSPISetup(0, 2000000) < 0)
	{
		perror("Erreur de setup du SPI");
		exit(EXIT_FAILURE);
	}

	uint8_t data[6] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (RUN >> 4) ;
	data[1] = ((RUN & 0x00F) << 4) & WRITE;
  	data[2] = 0x00;
  	data[3] = 0x00;



  	printf("on envoie : 0x%02X %02X %02X %02X\n\n", data[3], data[2], data[1], data[0]);
  
  	while(digitalRead(IRQ1));

	expander_t *exp = expander_init(EXPANDER_2);
	waitForReady(exp);
	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  	// expander_resetAllPinsGPIO(exp);
  	setAllCS(exp);

	// cs de Temperature adc a 0 uniquement lui les autres 1 
	usleep(1);
	
  	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);

	wiringPiSPIDataRW(0, data,4);

  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);

	usleep(1);

  	expander_closeAndFree(exp);

}



uint16_t ADE9078_getVersion(){
       
	if(wiringPiSPISetup(0, 2000000) < 0)
	{
		perror("Erreur de setup du SPI");
		exit(EXIT_FAILURE);
	}

	uint8_t data[6] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (VERSION_16 >> 4) ;
	data[1] = ((VERSION_16 & 0x00F) << 4) | READ;
  // data[2] = 0x00;
  // data[3] = 0x01;



  printf("on envoie: 0x%02X %02X\n", data[1], data[0]);
  
  	while(digitalRead(IRQ1));
	expander_t *exp = expander_init(EXPANDER_2);

	waitForReady(exp);
	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  	// expander_resetAllPinsGPIO(exp);
  	setAllCS(exp);

	// cs de Temperature adc a 0 uniquement lui les autres 1 
	usleep(1);
	
  	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);

	wiringPiSPIDataRW(0, data,4);

  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);

	usleep(1);



  printf("recu : 0x%02X %02X\n",data[3], data[2]);

  uint16_t recu = data[3] + (data[2] << 8);

  printf("VERSION : 0x%02X\n\n", recu); 
  expander_closeAndFree(exp);

  return recu;
}



uint32_t ADE9078_getPartID(){
       
	if(wiringPiSPISetup(0, 2000000) < 0)
	{
		perror("Erreur de setup du SPI");
		exit(EXIT_FAILURE);
	}

	uint8_t data[6] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (PART_ID >> 4) ;
	data[1] = ((PART_ID & 0x00F) << 4) | READ;
  // data[2] = 0x00;
  // data[3] = 0x01;



  
  	while(digitalRead(IRQ1));

	expander_t *exp = expander_init(EXPANDER_2);
	waitForReady(exp);
	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  // expander_resetAllPinsGPIO(exp);
  	setAllCS(exp);

	// cs de Temperature adc a 0 uniquement lui les autres 1 
	usleep(1);
	
  	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);

	wiringPiSPIDataRW(0, data,6);

  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);

	usleep(1);




  uint32_t recu = data[5] + (data[4] << 8) + (data[3] << 16) + (data[2] << 24);

  printf("part ID : 0x%X\n\n", recu); 
  expander_closeAndFree(exp);

  return recu;
}


int main(){


    ADE9078_setRun();
    ADE9078_getRun();
    while(1)
    {
      ADE9078_getVersion();
      ADE9078_getPartID();
      usleep(500000);
    }

    


  return EXIT_SUCCESS;
}