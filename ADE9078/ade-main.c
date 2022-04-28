#include "../lib/expander-i2c.h"


#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
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

static void pabort(const char *s)
{
        perror(s);
        abort();
}

const uint8_t WRITE = 0b00000000; //This value tells the ADE9078 that data is to be written to the requested register.
const uint8_t READ = 0b10000000;  //This value tells the ADE9078 that data is to be read from the requested register.

//////////
// Init SPIdev
//////////
 
//////////
// Read n bytes from the 2 bytes add1 add2 address
//////////

// uint8_t functionBitVal(uint16_t addr, uint8_t byteVal)
// {
// //Returns as integer an address of a specified byte - basically a byte controlled shift register with "byteVal" controlling the byte that is read and returned
//   uint16_t x = ((addr >> (8*byteVal)) & 0xff);

//   #ifdef ADE9078_VERBOSE_DEBUG
//    printf(" ADE9078::functionBitVal function (separates high and low command bytes of provided addresses) details: ");
//    printf(" Address input (dec): ");
//    printf("%d\n",addr);
//    printf(" Byte requested (dec): ");
//    printf("%d\n", byteVal);
//    printf(" Returned Value (dec): ");
//    printf("%d\n", x);
//    printf(" Returned Value (HEX): ");
//    printf("%02x\n", x);
//    printf(" ADE9078::functionBitVal function completed ");
//   #endif

//   return x;
// }

// uint16_t ADE9078_spiRead16(uint16_t address) { //This is the algorithm that reads from a register in the ADE9078. The arguments are the MSB and LSB of the address of the register respectively. The values of the arguments are obtained from the list of functions above.
    
//     #ifdef ADE9078_VERBOSE_DEBUG
//      printf(" ADE9078::spiRead16 function started \n");
//     #endif
//    //Prepare the 12 bit command header from the inbound address provided to the function

//     expander_t *exp = expander_init(0x27);

// 	  uint8_t ancienne_config = expander_getAllPinsGPIO(exp);
//     expander_setAllPinsGPIO(exp);

//    uint16_t temp_address, readval_unsigned;
//    temp_address = (((address << 4) & 0xFFF0)+8); //shift address  to align with cmd packet, convert the 16 bit address into the 12 bit command header. + 8 for isRead versus write
//    uint8_t commandHeader1 = functionBitVal(temp_address, 1); //lookup and return first byte (MSB) of the 12 bit command header, sent first
//    uint8_t commandHeader2 = functionBitVal(temp_address, 0); //lookup and return second byte (LSB) of the 12 bit command header, sent second

//     uint8_t one, two; //holders for the read values from the SPI Transfer


//     //expander_printGPIO(exp);

//   	expander_resetOnlyPinSetOthersGPIO(exp, 4);
//     spiData data;
// 	  spiTransfer(&data);

// 	    expander_setAndResetSomePinsGPIO(exp, ancienne_config);
//       expander_resetOnlyPinSetOthersGPIO(exp, 5);
//       // bcm2835_spi_transfer(commandHeader1); //Send MSB
//       // bcm2835_spi_transfer(commandHeader2); //Send MSB
//       // one = bcm2835_spi_transfer(WRITE);  //dummy write MSB, read out MSB
//       // two = bcm2835_spi_transfer(WRITE);  //dummy write LSB, read out LSB
//       // expander_setPinGPIO(exp,5);
//       // bcm2835_spi_end();


// 	#ifdef AVRESP8266 //Arduino SPI Routine
//     // beginTransaction is first
//     SPI.beginTransaction(defaultSPISettings);  // Clock is high when inactive. Read at rising edge: SPIMODE3.
//     digitalWrite(_SS, LOW);  //Enable data transfer by bringing SS line LOW
//     SPI.transfer(commandHeader1);  //Transfer first byte (MSB), command
//     SPI.transfer(commandHeader2);  ;//Transfer second byte (LSB), command
//     //Read in values sequentially and bitshift for a 32 bit entry
//     one = SPI.transfer(dummyWrite);  //MSB Byte 1  (Read in data on dummy write (null MOSI signal))
//     two = SPI.transfer(dummyWrite);  //LSB Byte 2  (Read in data on dummy write (null MOSI signal))
//     digitalWrite(_SS, HIGH);  //End data transfer by bringing SS line HIGH
//     SPI.endTransaction();      // end SPI Transaction
// 	#endif

//     #ifdef ADE9078_VERBOSE_DEBUG
//      printf(" ADE9078::spiRead16 function details: \n");
//      printf(" Command Header: \n");
//      printf("%02x\n",commandHeader1);
//      printf("%02x\n",commandHeader2);
//      printf(" Address Byte 1(MSB)[HEX]: \n");
//      printf(" Returned bytes (1(MSB) and 2) [HEX]: \n");
//      printf("%02x", one); //print MSB
//      printf("\n");
//      printf("%02x\n", two);  //print LSB
//      printf(" ADE9078::spiRead16 function completed \n");
//     #endif

// 	  readval_unsigned = (one << 8);  //Process MSB  (Alternate bitshift algorithm)
//     readval_unsigned = readval_unsigned + two;  //Process LSB
// 	return readval_unsigned;
// }

/*
  printf("ADE73370\t: %d\n",(data.rx[3] >> 21) & 0x01);
  printf("ADE9000\t: %d\n",(data.rx[3] >> 20) & 0x01);
  printf("ADE9004\t: %d\n",(data.rx[3] >> 16) & 0x01);
  */
void setAllCS(expander_t *exp)
{
  expander_setPinGPIO(exp, 2);
  expander_setPinGPIO(exp, 3);
  expander_setPinGPIO(exp, 4);
  expander_setPinGPIO(exp, 5);
}

uint16_t ADE9078_getVersion(){
       
	if(wiringPiSPISetup(0, 2000000) < 0)
	{
		perror("Erreur de setup du SPI");
		exit(EXIT_FAILURE);
	}

	uint8_t data[3] = {0};

	
    //0x4FE << 4 = 0x4FE0  = 0x4fe8 = 0x4F,                             16
	data[0] = ((VERSION_16 << 4) | 0b1000) >> 8 ;
	data[1] = 0xFF & ((VERSION_16 << 4) | 0b1000);
	data[1] = 0x00;
	data[2] = 0x00;

  printf("%x %x\n", data[0], data[1]);
  
  while(!digitalRead(IRQ1));

	expander_t *exp = expander_init(EXPANDER_2);

	uint8_t ancienne_config = expander_getAllPinsGPIO(exp);


	// cs de Temperature adc a 0 uniquement lui les autres 1 
	expander_resetOnlyPinSetOthersGPIO(exp, PM_CS);
	
	usleep(1);

	wiringPiSPIDataRW(0, data, 4);

	expander_setAndResetSomePinsGPIO(exp, ancienne_config);

	usleep(1);



  printf("version : %X %X\n",data[1], data[0]);
  expander_closeAndFree(exp);

  return 1;
}



int main(){


    ADE9078_getVersion();


    


  return 0;
}