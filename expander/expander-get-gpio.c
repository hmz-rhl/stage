#include "../lib/expander-i2c.h"
#include <math.h>


int main(int argc, char *argv[]){

    if(argc != 2){
    
        printf("Usage: ./get-expander1-gpio <addresse> \n");
        printf("exemple: ./set_expander1_gpio 26 \n");
        exit(EXIT_FAILURE);

    }
  
     uint8_t addr;
    if(!strcmp(argv[1], "20")){

        addr = 0x20;
        
    }
    
    else if(!strcmp(argv[1], "21")){

        addr = 0x21;
        
    }
    
    else if(!strcmp(argv[1], "22")){

        addr = 0x22;
        
    }
    
    else if(!strcmp(argv[1], "23")){

        addr = 0x23;
        
    }
    
    else if(!strcmp(argv[1], "24")){

        addr = 0x24;
        
    }
    
    else if(!strcmp(argv[1], "25")){

        addr = 0x25;
        
    }

    
    else if(!strcmp(argv[1], "26")){

        addr = 0x26;
        
    }
    
    else if(!strcmp(argv[1], "27")){

        addr = 0x27;
        
    }
    else{

        printf("fonction %s : addresse doit etre entre 20 et 27\n", __func__);
        exit(EXIT_FAILURE);
    }
    
    expander_t *e = expander_init(addr);

    expander_printGPIO(e);
    expander_closeAndFree(e);

    return 0;
}
