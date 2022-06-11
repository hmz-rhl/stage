#include "../lib/expander-i2c.h"
#include "../lib/ADE9078registers.h"

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
#include <signal.h>

//#define DEBUG

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define VERSION_16       0x4FE //Reset: 0x0040 Access: R

#define ADE9078_VERBOSE_DEBUG

#define EXPANDER_1      0x26  // Adresse de l'expander 1 d'Output

#define EXPANDER_2      0x27  // Adresse de l'expander 2 d'Output

#define IRQ1              5
#define IRQ0              6


#define RUN             0x480

#define READ            0x0008
#define WRITE            0xFFF7


#define AVrmsGain 	1
#define AVrmsOffset 0
#define AIrmsGain 	1
#define AIrmsOffset 0

#define AAppPowerGain 1
#define AAppPowerOffset 0




//#define ADE9078_VERBOSE_DEBUG

typedef struct {
  // All gains are 2 bits. Options: 1, 2, 3, 4
  uint8_t vAGain;
  uint8_t vBGain;
  uint8_t vCGain;

  uint8_t iAGain;
  uint8_t iBGain;
  uint8_t iCGain;
  uint8_t iNGain;

  uint32_t powerAGain;
  uint32_t powerBGain;
  uint32_t powerCGain;

  uint8_t vConsel;
  uint8_t iConsel;
  int fd;
}InitializationSettings;


InitializationSettings is;

double decimalize(uint32_t input, double factor, double offset, int absolutevalue) //This function converts to floating point with an optional linear calibration (y=mx+b) by providing input in the following way as arguments (rawinput, gain, offset)
{
	#ifdef ADE9078_VERBOSE_DEBUG
	printf(" ADE9078::calibration (decimalize) and double type conversion function executed, RAW input: ");
	printf("%d\n", input);
	#endif
	#ifdef ADE9078_Calibration
	printf(" ADE9078::calibration (decimalize) and double type conversion function executed, RAW input: ");
	printf("%d\n", input);
	#endif
	//Warning, you can get overflows due to the printout of returned values in Arduino, See: http://forum.arduino.cc/index.php/topic,46931.0.html
	if(absolutevalue == 0){
	return (((double)input*factor)+offset); //standard y=mx+b calibration function applied to return
	}
	else{
		return (abs(((double)input*factor)+offset)); //standard y=mx+b calibration function applied to return
	}
}



void setAllCS(expander_t *exp)
{
  expander_setPinGPIO(exp, 2);
  expander_setPinGPIO(exp, 3);
  expander_setPinGPIO(exp, 4);
  expander_setPinGPIO(exp, 5);
}


void waitForSPIReady(expander_t *exp){
	
	// if((expander_getAllPinsGPIO(exp) & (uint8_t)0b11000000) == 0b11000000)
	// {
	// 	printf(" l'ADE est en PSM3 (idle mode) : donc pas fonctionnel veuillez le set a PSM0/PSM1/PSM2/\n");
	// 	exit(EXIT_FAILURE);
	// }
	time_t start, end;
	double attente = 0;
	start = clock();
	while(( expander_getAllPinsGPIO(exp) & (uint8_t)0b00111100 != 0b00111100 ))
	{
		end = clock();
		attente = 100 *(double)(end - start) / (double)(CLOCKS_PER_SEC);
		if(attente > 20)
		{
			perror("Erreur timeout: SPI busy tout les CS ne sont pas a 1");
			exit(EXIT_FAILURE);
		}
	}
}


uint16_t spiRead16(uint16_t addresse){
       

	uint8_t data[6] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (addresse >> 4) ;
	data[1] = ((addresse & 0x00F) << 4) | READ;
  // data[2] = 0x00;
  // data[3] = 0x01;
#ifdef DEBUG
	printf("\n\non envoit %02X%02X sur SPI \n", data[0], data[1]);
#endif	




  
	expander_t *exp = expander_init(EXPANDER_2);
	waitForSPIReady(exp);



	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  // expander_resetAllPinsGPIO(exp);
  	setAllCS(exp);

	// cs de Temperature adc a 0 uniquement lui les autres 1 
	usleep(1);
	
  	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);

#ifdef DEBUG
  	printf("|read register %x|\n", addresse);
#endif
while(digitalRead(IRQ0)){}
	wiringPiSPIDataRW(0, data,6);

	usleep(1);
  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);





  	uint32_t recu = data[3] + (data[2] << 8);

#ifdef DEBUG
  	printf("recue : %04x\n", recu);
#endif

  	expander_closeAndFree(exp);

  return recu;
}


uint32_t spiRead32(uint16_t addresse){
       

	uint8_t data[10] = {0};
	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (addresse >> 4) ;
	data[1] = ((addresse & 0x00F) << 4) | READ;
	//uint16_t envoi = data[0] << 8 + data[1];

#ifdef DEBUG
	printf("\n\non envoit %02X%02X sur SPI \n", data[0], data[1]);
#endif	
  // data[2] = 0x00;
  // data[3] = 0x01;



  
	expander_t *exp = expander_init(EXPANDER_2);
	waitForSPIReady(exp);




	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  	// expander_resetAllPinsGPIO(exp);
  	setAllCS(exp);

	// cs de Temperature adc a 0 uniquement lui les autres 1 
	usleep(1);
	
  	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);

#ifdef DEBUG
  	printf("|read register %x|\n", addresse);
#endif
// while(digitalRead(IRQ0)){}
	wiringPiSPIDataRW(0, data,10);

	usleep(1);
  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);





	uint32_t recu = data[5] + (data[4] << 8) + (data[3] << 16) + (data[2] << 24);

#ifdef DEBUG
  	printf("Recu : %08X\n", recu);
#endif
	// printf("on recoit %d\n", data && 0xFFFF);
  	expander_closeAndFree(exp);

  return recu;
}


void spiWrite16(uint16_t addresse, uint16_t value){

	uint8_t data[6] ={0};
	data[0] = 0x00FF & (addresse >> 4) ;
	data[1] = ((addresse & 0x00F) << 4) & WRITE;
	data[2] = 0x00FF & (value >> 8) ;
	data[3] = ((value & 0x00FF)) ;
  	
#ifdef DEBUG
	printf("\n\non envoit %02X%02X %02X%02X sur SPI \n", data[0], data[1], data[2], data[3]);
#endif	


#ifdef DEBUG
#endif
// on attend que irq1 passe a 0

	expander_t *exp = expander_init(EXPANDER_2);

	// on attend que tout les CS se libere pour eviter d'entrer en conflit sur le bus spi
	// on si on depasse un certain timeout on return
	
	waitForSPIReady(exp);


	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  // expander_resetAllPinsGPIO(exp);
 	setAllCS(exp);

	usleep(1);
	
	// cs de Temperature ADE a 0 uniquement lui les autres 1 
 	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);
#ifdef DEBUG
 	printf("|write %x on register %x|\n", value, addresse);
#endif
while(digitalRead(IRQ0)){}
	wiringPiSPIDataRW(0, data,6);

	usleep(1);
  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);
	if(spiRead16(addresse) != value){

		printf("%s: Error %04X n'a pas été écris dans %04X\n", __func__, value,  addresse);
	}
	else{
		printf("%s: Success %04X a été écris dans %04X\n", __func__, value,  addresse);
	}


  	expander_closeAndFree(exp);

}



void spiWrite32(uint16_t addresse, uint32_t value){


	uint8_t data[10] = {0};
	data[0] = 0x00FF & (addresse >> 4) ;
	data[1] = ((addresse & 0x00F) << 4) & WRITE;
	data[2] = 0x00FF & (value >> 24) ;
	data[3] = 0x00FF & (value >> 16);
  	data[4] = 0x00FF & (value >> 8) ;
	data[5] = ((value & 0x00FF)) ;

#ifdef DEBUG
	printf("\n\non envoit %02X%02X %02X%02X%02X%02X sur SPI \n", data[0], data[1], data[2], data[3], data[4], data[5]);
#endif	

  	


#ifdef DEBUG
#endif
// on attend que irq1 passe a 0

	expander_t *exp = expander_init(EXPANDER_2);

	// on attend que tout les CS se libere pour eviter d'entrer en conflit sur le bus spi
	// on si on depasse un certain timeout on return
	
	waitForSPIReady(exp);


	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  // expander_resetAllPinsGPIO(exp);
 	setAllCS(exp);

	usleep(1);
	
	// cs de Temperature ADE a 0 uniquement lui les autres 1 
 	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);
#ifdef DEBUG
 	printf("|write %08X on register %04x|\n", value, addresse);
#endif
while(digitalRead(IRQ0)){}
	wiringPiSPIDataRW(0, data,10);

	usleep(1);
  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);


  	expander_closeAndFree(exp);

	if(spiRead32(addresse) != value){

		printf("%s: Error %08X n'a pas été écris dans %04X\n", __func__, value,  addresse);
	}
	else{
		printf("%s: Success %08X a été écris dans %04X\n", __func__, value,  addresse);
	}

}


void ADE9078_PSM0(){
	
	expander_t *exp = expander_init(EXPANDER_2);

	expander_resetPinGPIO(exp, PM0);
	expander_resetPinGPIO(exp, PM1);

}

void ADE9078_PSM1(){
	
	expander_t *exp = expander_init(EXPANDER_2);

	expander_setPinGPIO(exp, PM0);
	expander_resetPinGPIO(exp, PM1);

}

void ADE9078_PSM2(){
	
	expander_t *exp = expander_init(EXPANDER_2);

	expander_resetPinGPIO(exp, PM0);
	expander_setPinGPIO(exp, PM1);

}

void ADE9078_PSM3(){
	
	expander_t *exp = expander_init(EXPANDER_2);

	expander_setPinGPIO(exp, PM0);
	expander_setPinGPIO(exp, PM1);

}

uint16_t ADE9078_getRun(){
       

	uint8_t data[6] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (RUN_16 >> 4) ;
	data[1] = ((RUN_16 & 0x00F) << 4) | READ;
  // data[2] = 0x00;
  // data[3] = 0x01;



  
	expander_t *exp = expander_init(EXPANDER_2);
	waitForSPIReady(exp);

  	while(digitalRead(IRQ0));

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

	usleep(1);
  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);





  	uint16_t recu = data[3] + (data[2] << 8);

#ifdef DEBUG
  	printf("RUN : %d\n", recu);
#endif

  	expander_closeAndFree(exp);

  return recu;
}

void ADE9078_setRun(){
       

	uint8_t data[6] = {0};

	

	data[0] = 0x00FF & (RUN_16 >> 4) ;
	data[1] = ((RUN_16 & 0x00F) << 4) & WRITE;
  	data[2] = 0x00;
  	data[3] = 0x01;


#ifdef DEBUG
#endif
// on attend que irq1 passe a 0

	expander_t *exp = expander_init(EXPANDER_2);

	// on attend que tout les CS se libere pour eviter d'entrer en conflit sur le bus spi
	// on si on depasse un certain timeout on return
	
	waitForSPIReady(exp);

 	while(digitalRead(IRQ0));
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

	usleep(1);
  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);


  	expander_closeAndFree(exp);

}

void ADE9078_resetRun(){
       

	uint8_t data[6] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (RUN_16 >> 4) ;
	data[1] = ((RUN_16 & 0x00F) << 4) & WRITE;
  	data[2] = 0x00;
  	data[3] = 0x00;



  	printf("on envoie : 0x%02X %02X %02X %02X\n\n", data[3], data[2], data[1], data[0]);
  

	expander_t *exp = expander_init(EXPANDER_2);
	waitForSPIReady(exp);

  	while(digitalRead(IRQ0));
	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  	// expander_resetAllPinsGPIO(exp);
  	setAllCS(exp);

	// cs de Temperature adc a 0 uniquement lui les autres 1 
	usleep(1);
	
  	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);

	wiringPiSPIDataRW(0, data,4);

	usleep(1);
  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);


  	expander_closeAndFree(exp);

}

void ADE9078_waitForResetDone(){

	printf("waiting reset...");
	while(spiRead32(STATUS1_32) & 0b00000000000000010000000000000000 != 0b00000000000000010000000000000000){usleep(100000);putchar('.');}
	printf("Reset done !\n");
}

uint16_t ADE9078_getVersion(){
       

	uint8_t data[6] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (VERSION_16 >> 4) ;
	data[1] = ((VERSION_16 & 0x00F) << 4) | READ;
  // data[2] = 0x00;
  // data[3] = 0x01;



  	printf("on envoie: 0x%02X %02X\n", data[1], data[0]);
  
	expander_t *exp = expander_init(EXPANDER_2);

	waitForSPIReady(exp);

  	while(digitalRead(IRQ0));
	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  	// expander_resetAllPinsGPIO(exp);
  	setAllCS(exp);

	// cs de Temperature adc a 0 uniquement lui les autres 1 
	usleep(1);
	
  	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);

	wiringPiSPIDataRW(0, data,4);

	usleep(1);
  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);




  printf("recu : 0x%02X %02X\n",data[3], data[2]);

  uint16_t recu = data[3] + (data[2] << 8);

  printf("VERSION : 0x%02X\n\n", recu); 
  expander_closeAndFree(exp);

  return recu;
}

uint32_t ADE9078_getPartID(){
       

	uint8_t data[6] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = 0x00FF & (PART_ID_32 >> 4) ;
	data[1] = ((PART_ID_32 & 0x00F) << 4) | READ;
  // data[2] = 0x00;
  // data[3] = 0x01;



  

	expander_t *exp = expander_init(EXPANDER_2);
	waitForSPIReady(exp);

  	while(digitalRead(IRQ0));
	// uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
  // expander_resetAllPinsGPIO(exp);
  	setAllCS(exp);

	// cs de Temperature adc a 0 uniquement lui les autres 1 
	usleep(1);
	
  	expander_resetPinGPIO(exp, PM_CS);
	
	usleep(1);

	wiringPiSPIDataRW(0, data,6);

	usleep(1);
  	expander_setPinGPIO(exp, PM_CS);

	// expander_setAndResetSomePinsGPIO(exp, ancienne_config);





  uint32_t recu = data[5] + (data[4] << 8) + (data[3] << 16) + (data[2] << 24);

  printf("part ID : 0x%02X\n\n", recu); 
  expander_closeAndFree(exp);

  return recu;
}

uint32_t ADE9078_getVpeak(){
	uint32_t value=0;
	value=spiRead32(VPEAK_32);
	return value;
}

uint32_t ADE9078_getInstVoltageA(){

	// while(((spiRead32(STATUS1_32) >> 16) & 0x01) != 1);
	uint32_t value=0;
	value=spiRead32(AV_PCF_32);
	return value;
}

// double ADE9078_getInstApparentPowerA(){
// 	uint32_t value=0;
// 	value=spiRead32(AVA_32);
// 	double decimal = decimalize(value, AAppPowerGain, AAppPowerOffset,0); //convert to double with calibration factors specified
// 	return (decimal);
// }

uint32_t ADE9078_getInstCurrentA(){

	// while(((spiRead32(STATUS1_32) >> 16) & 0x01) == 1);
	uint32_t value=0;
	value=spiRead32(AI_PCF_32);
	return value;
}

void ADE9078_configureWFB(){
	int i;
	uint16_t writeValue = spiRead16(WFB_CFG_16);
	//stop filling buffer to config buffer
  	writeValue &= ~(0b1 << 4);  //set WF_CAP_EN bit to 0 in the WFB_CFG register
  	spiWrite16(WFB_CFG_16, writeValue);

	//enable INeutral: uncomment the following
	writeValue = (writeValue | (0b1 << 12));
	//disable INeutral: uncomment the following
	//writeValue = (writeValue & ~(-0b1<<12));

	//sinc4output
	writeValue = (writeValue & ~(0b1 << 8));//WF_SRC bit to 00
  	writeValue = (writeValue & ~(0b1 << 9));
	//mode 0//Stop when buffer is full mode
	writeValue &= ~(0b1<<6); //WF_MODE bits = 00
	writeValue &= ~(0b1<<7);
	//read resampled data
	writeValue &= ~(0b1<<5); //WF_CAP_SEL = 0
	//burstAllChannels
	for (i = 0; i == 3; i++){
		writeValue = writeValue & ~(0b1<<i);
	}

	printf("writing to WFB_CFG: ");
	printf("%d  \n",writeValue);
	spiWrite16(WFB_CFG_16, writeValue);
	//printf("WFB configured\n");
}

int ADE9078_isDoneSampling()
{
		int check = 0;
		uint32_t status = spiRead32(STATUS0_32);
		//printf(status,BIN);
		// 23th bit tells you that the buffer is full
		status = (status >> 23);
		check = (status & 0b1);
		//printf(check);
		return check;
}

void ADE9078_stopFillingBuffer(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent = (addressContent & ~(0b1 << 4));  //set WF_CAP_EN bit to 0 in the WFB_CFG register

	// printf("WFB_CFG disable\t\n");
	// printf(addressContent,BIN);

	spiWrite16(WFB_CFG_16, addressContent);
  // printf("wfb stopped\n");
}

//this resets the bit in STATUS0_32 called COH_WFB_FULL so we can once again be notified if the thing is full
void ADE9078_resetFullBufferBit(){
	uint32_t addressContent = spiRead32(STATUS0_32);
	// printf("buff check read\t\n");
	// printf(addressContent,BIN);
	addressContent = (addressContent & ~(0b1 << 23));
	// printf("buff check writ\t\n");
	// printf(addressContent,BIN);
	spiWrite32(STATUS0_32, addressContent);
}
//Start the WFB
void ADE9078_startFillingBuffer(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent = (addressContent | (0b1 << 4));  //set WF_CAP_EN bit to 1 in the WFB_CFG register to start filling the buffer from Address 0x800.

	// printf("WFB_CFG enable\t\n");
	// printf(addressContent,BIN);

	spiWrite16(WFB_CFG_16, addressContent);
  //printf("wfb fill start\n");
}

void ADE9078_waitForDataReady(){

	while(digitalRead(25));
}

void ADE9078_initialize(InitializationSettings *is){


  #ifdef ADE9078_VERBOSE_DEBUG
   printf("initialize function started\n"); //wiring configuration defined in VCONSEL and ICONSEL registers init. in this function
  #endif
  	is->fd = wiringPiSPISetup(0, 5000000);
	
	if(is->fd < 0)
	{
		perror("Erreur de setup du SPI");
		exit(EXIT_FAILURE);
	}

	if(is == NULL)
	{
		printf("Erreur %s : argument NULL", __func__);
		exit(EXIT_FAILURE);
	}
	while(digitalRead(IRQ0)){}
  	spiWrite16(CONFIG1_16, 0x0001); // software reset

	usleep(100000);
  // on attend le reset
  ADE9078_waitForResetDone();

//   // Page 56 of datasheet quick start
//   // #1: Ensure power sequence completed
  
  
//   	sleep(150);

while(digitalRead(IRQ0)){}
  spiWrite16(CONFIG0_32, 0x00000010);

while(digitalRead(IRQ0)){}
  spiWrite16(CONFIG1_16, 0b1001001100001100);

while(digitalRead(IRQ0)){}
  spiWrite32(AIGAIN_32, 0x00000001);
while(digitalRead(IRQ0)){}
  spiWrite16(CONFIG2_16, 0x0000);
while(digitalRead(IRQ0)){}
  spiWrite16(CONFIG3_16, 0x0000);
// //   // Is always printing right now. Might be an issue?
// //   // if (!checkBit((int)read32BitAndScale(STATUS1_32), 16)) {
// //   //   printf("WARNING, POWER UP MAY NOT BE FINISHED\n");
// //   // }


// // // #2: Configure Gains
while(digitalRead(IRQ0)){}
   spiWrite32(APGAIN_32, is->powerAGain);
while(digitalRead(IRQ0)){}
   spiWrite32(BPGAIN_32, is->powerBGain);
while(digitalRead(IRQ0)){}
   spiWrite32(CPGAIN_32, is->powerCGain);

   	spiWrite32(AIGAIN_32, 2);
	   while(digitalRead(IRQ0)){}
	spiWrite32(BIGAIN_32, 2);
	while(digitalRead(IRQ0)){}
	spiWrite32(CIGAIN_32, 2);
	while(digitalRead(IRQ0)){}
	spiWrite32(NIGAIN_32, 2);
	while(digitalRead(IRQ0)){}
	spiWrite32(AVGAIN_32, 2);
while(digitalRead(IRQ0)){}
	spiWrite32(BVGAIN_32, 2);
	while(digitalRead(IRQ0)){}
	spiWrite32(CVGAIN_32, 2);

//    uint16_t pgaGain = (is->vCGain << 12) + (is->vBGain << 10) + (is->vCGain << 8) +   // first 2 reserved, next 6 are v gains, next 8 are i gains.
//                       (is->iNGain << 6) + (is->iCGain << 4) + (is->iBGain << 2) + is->iAGain;
while(digitalRead(IRQ0)){}
   spiWrite16(PGA_GAIN_16, 0b0001010101010101);

while(digitalRead(IRQ0)){}
  spiWrite16(EGY_TIME_16, 0x0001); // update time accumulation

//   while(digitalRead(IRQ0)){}
//   spiWrite16(WFB_CFG_16, 0b0001000011011111);

// // // on veut activer l'interruption losqu'une donnee est disp dans le waveform buffer
    while(digitalRead(IRQ0)){}
  spiWrite16(MASK0_32, 0b00000000000000001000000000000000);

    while(digitalRead(IRQ0)){}
  spiWrite16(WFB_CFG_16, 0b0000000000001111);

// // // #5 : Write VLevel 0x117514
   uint32_t vLevelData = 0x117514;//0x35A98F;  
while(digitalRead(IRQ0)){}
   spiWrite32(VLEVEL_32, vLevelData); // #5


// //   /*
// //   Potentially useful registers to configure:
// //   The following were in the 9078:
// //     0x49A ZX_LP_SEL : to configure "zero crossing signal"
// //     0x41F PHNOLOAD : To say if something is "no load".
// //     Phase calibrations, such as APHCAL1_32
// //   */

// // /* #6: If a Rogowski coil sensor is used, write the INTEN bit in
// // 	the CONFIG0 register to enable the digital integrator on
// // 	the IA, IB, and IC channels. To enable the digital integrator on
// // 	the neutral current, IN, channel, set the ININTEN bit.
// // 	Additionally, write DICOEF = 0xFFFFE000 to configure
// // 	the digital integrator. If current transformers are used,
// // 	INTEN and ININTEN in the CONFIG0 register must = 0.*/

while(digitalRead(IRQ0)){}
  spiWrite32(DICOEFF_32, 0xFFFFE000); // Recommended by datasheet

// // // #7:  If current transformers are used, INTEN and ININTEN in the CONFIG0 register must = 0
// //   // Table 24 to determine how to configure ICONSEL and VCONSEL in the ACCMODE register

  uint16_t settingsACCMODE = 0x0020;// 0x0020;//(is->iConsel << 6) + (is->vConsel << 5);

while(digitalRead(IRQ0)){}
  spiWrite16(ACCMODE_16, 0b0000000000000000); // chooses the wiring mode (delta/Wye, Blondel vs. Non-blondel) to push up in initial config, Need the other if statements for all configuration modes


// 8: Write 1 to Run register
while(digitalRead(IRQ0)){}
	spiWrite16(RUN_16, 1);  


// 9: Write 1 to EP_CFG register
while(digitalRead(IRQ0)){}
  	spiWrite16(EP_CFG_16, 1);  // RD_EST_EN=1, EGY_LD_ACCUM=0, EGY_TMR_MODE=0, EGY_PWR_EN=1
  
  /* Registers configured in ADE9000 code */
  // zx_lp_sel
  // mask0, mask1, event_mask,
  // wfb_cfg,



//   #ifdef ADE9078_VERBOSE_DEBUG
//    printf(" ADE9078:initialize function completed. Showing values and registers written \n");
//    printf(" APGAIN: ");
//    printf("%d \n",is->powerAGain);
//    printf(" BPGAIN: ");
//    printf("%d \n",is->powerBGain);
//    printf(" CPGAIN: ");
//    printf("%d \n",is->powerCGain);
//    printf(" PGA_GAIN: ");
//    printf("%d \n",pgaGain);
//    printf(" VLEVEL: ");
//    printf("%d \n",vLevelData);
//    printf(" CONFIG0-3, ALL 0'Sn \n");
//    printf(" ACCMODE: ");
//    printf("%d \n",settingsACCMODE);
//    printf(" RUN: ");
//    printf("1\n");
//    printf(" EP_CFG: ");
//    printf("1\n");
//    printf(" DICOEFF: ");
//    printf("0xFFFFE000\n");
//   #endif
  
}

uint32_t ADE9078_getAVrms(){

	uint32_t value=0;
	value=spiRead32(AVRMS_32);
	double decimal = decimalize(value, AVrmsGain, AVrmsOffset,0); //convert to double with calibration factors specified, no abs value
	return value;
}

uint32_t ADE9078_getAIrms(){

	uint32_t value=0;
	value=spiRead32(AIRMS_32);
	double decimal = decimalize(value, AIrmsGain, AIrmsOffset,0); //convert to double with calibration factors specified, no abs value
	return value;
}

double ADE9078_getInstApparentPowerA(){
	uint32_t value=0;
	value=spiRead32(AVA_32);
	double decimal = decimalize(value, AAppPowerGain, AAppPowerOffset,0); //convert to double with calibration factors specified
	return (decimal);
}

uint32_t ADE9078_getPhaseCalibA(){
	uint32_t value=0;
	value=spiRead32(APHCAL0_32);
	return value;
}

void interruption(int n)
{
	expander_t *exp = expander_init(0x26);
	// ouverture des relais
	expander_resetPinGPIO(exp, TYPE_E_F_ON);
	expander_resetPinGPIO(exp, TYPE_2_NL1_ON);
	expander_resetPinGPIO(exp, TYPE_2_L2L3_ON);
	close(is.fd);
	expander_closeAndFree(exp);
	exit(EXIT_SUCCESS);
}
void print32bits(int v) {
  int i; // for C89 compatability
  for(i = 31; i >= 0; i--) putchar('0' + ((v >> i) & 1));
  putchar('\n');
}

void print16bits(int v) {
  int i; // for C89 compatability
  for(i = 15; i >= 0; i--) putchar('0' + ((v >> i) & 1));
  putchar('\n');
}

int main(){

	expander_t *exp = expander_init(0x26);
	// fermeture du relais L1N
	expander_setPinGPIO(exp, TYPE_E_F_ON);
	expander_setPinGPIO(exp, TYPE_2_L2L3_ON);
	expander_setPinGPIO(exp, TYPE_2_NL1_ON);
	expander_closeAndFree(exp);


	signal(SIGABRT, interruption);
	signal(SIGINT, interruption);

		
		is.vAGain=2;
		is.vBGain=2;
		is.vCGain=2;

		is.iAGain=2;
		is.iBGain=2;
		is.iCGain=2;
		is.iNGain=2;

		is.powerAGain=1;
		is.powerBGain=1;
		is.powerCGain=1;

		//Use these settings to configure wiring configuration at stertup
		//FYI: B010=2 in DEC
		//FYI: B100 = 4 in DEC

		//4 Wire Wye configuration - non-Blondel compliant:
		is.vConsel=0;
		is.iConsel=0;


	sleep(1);
	//ADE9078_PSM0();
	sleep(1);
	ADE9078_PSM1();
	//ADE9078_resetRun();
    ADE9078_initialize(&is);
	printf("Burst : %x\n",spiRead16(WFB_CFG_16));
	ADE9078_getVersion();
	ADE9078_getPartID();
	while(digitalRead(IRQ0)){
		
	}

	// affichage des gains actuelles
	printf("phase calib A : %d\n", ADE9078_getPhaseCalibA());
	printf("AIGAIN : %d\n", spiRead32(AIGAIN_32));
	printf("BIGAIN : %d\n", spiRead32(BIGAIN_32));
	printf("CIGAIN : %d\n", spiRead32(CIGAIN_32));
	printf("NIGAIN : %d\n", spiRead32(NIGAIN_32));
	printf("AVGAIN : %d\n", spiRead32(AVGAIN_32));
	printf("BVGAIN : %d\n", spiRead32(BVGAIN_32));
	printf("CVGAIN : %d\n", spiRead32(CVGAIN_32));
	printf("ADC_REDIRECT : ");
	print32bits(spiRead32(ADC_REDIRECT_32));



	while(1){

		// while(digitalRead(IRQ0) && !digitalRead(25)){}
// 		usleep(20);
// 		printf("\nPHNOLOAD :\t ");
// 		print32bits(spiRead32(PHNOLOAD_32));
// 		printf("LAST_CMD: %04X\n", spiRead16(LAST_CMD_16));

// 		printf("\nSTATUS0 :\t ");
// 		print32bits(spiRead32(STATUS0_32));
// 		printf("LAST_CMD: %04X\n", spiRead16(LAST_CMD_16));

// 		printf("\nMASK0 :  \t ");
// 		print32bits(spiRead32(MASK0_32));
// 		printf("LAST_CMD: %04X\n", spiRead16(LAST_CMD_16));

		printf("\nSTATUS1 :\t ");
		print32bits(spiRead32(STATUS1_32));
// 		printf("LAST_CMD: %04X\n", spiRead16(LAST_CMD_16));
		
// 		printf("\nMASK1 :  \t ");
// 		print32bits(spiRead32(MASK1_32));
// 		printf("LAST_CMD: %04X\n", spiRead16(LAST_CMD_16));

// 		printf("\nEVENT_STATUS :\t ");
// 		print32bits(spiRead32(EVENT_STATUS32));
// 		printf("LAST_CMD: %04X\n", spiRead16(LAST_CMD_16));

// 		printf("\nEVENT_MASK :\t ");
// 		print32bits(spiRead32(EVENT_MASK_32));
// //		ADE9078_waitForDataReady();
		
// 		printf("\n\nCV_PCF : %d\n", spiRead32(CV_PCF_32));
// 		printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
// 		ADE9078_waitForDataReady();
		
// 		printf("\nBV_PCF : %d\n", spiRead32(BV_PCF_32));
// 		printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
// //		ADE9078_waitForDataReady();

// 		printf("\nCV_PCF : %d\n", spiRead32(CV_PCF_32));
		// printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
		// ADE9078_waitForDataReady();

		printf("%d\n", spiRead32(AV_PCF_32));
		// printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
// //		ADE9078_waitForDataReady();

// 		printf("\nBI_PCF : %d\n", spiRead32(BI_PCF_32));
// 		printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
// //		ADE9078_waitForDataReady();

// 		printf("\nCI_PCF : %d\n", spiRead32(CI_PCF_32));
// 		printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
// //		ADE9078_waitForDataReady();

// 		printf("\nNI_PCF : %d\n", spiRead32(NI_PCF_32));
// 		printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
// //		ADE9078_waitForDataReady();

		// ADE9078_waitForDataReady();
		// printf("\n\nAVRMS : %d\n", spiRead32(AVRMS_1_32));
		// printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
		
// 		printf("\nBVRMS : %d\n", spiRead32(BVRMS_32));
// 		printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
// //		ADE9078_waitForDataReady();

// 		printf("\nCVRMS : %d\n", spiRead32(CVRMS_32));
// 		printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));

		// ADE9078_waitForDataReady();
		// printf("\nAIRMS : %d\n", spiRead32(AIRMS_1_32));
		// printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
//		ADE9078_waitForDataReady();

// 		printf("\nBIRMS : %d\n", spiRead32(BIRMS_32));
// 		printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
// //		ADE9078_waitForDataReady();

// 		printf("\nCIRMS : %d\n", spiRead32(CIRMS_32));
// 		printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
// //		ADE9078_waitForDataReady();

// 		printf("\nNIRMS : %d\n", spiRead32(NIRMS_32));
// 		printf("LAST_DATA: %08X\n", spiRead32(LAST_DATA_32_32));
// //		ADE9078_waitForDataReady();


		
      	// sleep(5);
    }

	close(is.fd);
    expander_resetPinGPIO(exp,TYPE_E_F_ON);

  return EXIT_SUCCESS;
}