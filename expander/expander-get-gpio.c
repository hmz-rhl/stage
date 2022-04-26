#include "../lib/expander-i2c.h"
#include <math.h>


int main(int argc, char *argv[]){

    if(argc != 2){
    
        printf("Usage: ./get-expander1-gpio <addresse> \n");
        printf("exemple: ./set_expander1_gpio 26 \n");
        exit(EXIT_FAILURE);

    }
    uint8_t addr = 0x00;

    addr = atoi(argv[1]);
     
    if(addr > 27 || addr < 20){

        printf("fonction %s : addresse doit etre entre 20 et 27\n", __func__);
        exit(EXIT_FAILURE);
    }
    
    expander_t *e = expander_init(addr);

    expander_printGPIO(e);
    expander_closeAndFree(e);

    return 0;
}
