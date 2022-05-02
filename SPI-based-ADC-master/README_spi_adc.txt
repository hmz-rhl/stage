mcp3202_adc is an application which does analog to digital conversion by taking the analog input from potentiometer connected to MCP3202 chip and displays its 
respective digital value.

Chip used:
MCP3202 12-Bit A/D Converter with SPI interface.

Kit:
SPI-ADC/DAC ADDON CARD

Hardware setup:
Make the following connections between MCP3202 chip pinouts to module
------------------------------------------------------------------------------------------------------------------------
MCP3202    				-	SPI Signal   			-	Evaluation Board V3.1A 
------------------------------------------------------------------------------------------------------------------------
 ADC_CS 	 			-	Chip Select (CS) 	 	-	SODIMM_86 / SSP_FRM
 ADC_SCK 	 			-	Serial Clock (SCLK) 	 	-	SODIMM_88 / SSP_SCLK
 ADC_SDO 	 			-	Master In Slave Out (MISO) 	-	SODIMM_90 / SSP_RXD
 +5V (2.7V - 5.5V operational range) 	-					-	3.3V
 GND 	 				-					-	GND
------------------------------------------------------------------------------------------------------------------------

Compiling the application:
In the application directory, run make command preceding machine(module) name.
e.g: MACHINE=colibri-t20 make
Copy the exe to the module either through ethernet and USB.
Ethernet:
scp mcp3202_adc root@10.18.0.162:/home/root
USB:
cp mcp3202_adc /media/USB

Run the application:
Run the application with application name followed by adc channel number and spi device name.

Output:
Vary the potentiometer knob provided in the SPI-ADC/DAC ADDON CARD, the output gives the digital value for the analog input from 
potentiometer. One can cross check the output with an oscilloscope by comparing the voltage level of analog input and its respective digital
output.

