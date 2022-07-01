#include <rtc_eeprom.h>
#include <expander-i2c.h>
#include <MCP3202.h>
#include <pn532.h>
#include <PN532_Rpi_I2C.h>

int main(int argc, char **argv){

    pritnf("test de la librairie de l'expander...\n");
    print("VERSION : %s\n", VERSION_EXPANDER_I2C);
    printf("test librarire expander-i2c.h fini \n");


    pritnf("test de la librairie de la rtc eeprom...\n");
    print("VERSION : %s\n", VERSION_RTC_EEPROM);
    printf("test librarire rtc_eeprom.h fini \n");

    pritnf("test de la librairie du MCP3202...\n");
    print("VERSION : %s\n", VERSION_MCP3202);
    printf("test librarire MCP3202.h fini \n");

    pritnf("test de la librairie du pn532...\n");
    print("VERSION : %s\n", VERSION_PN532);
    printf("test librarire pn532.h PN532_Rpi_I2C.h fini \n");
    

    return 0;

}