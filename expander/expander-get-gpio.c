/**
 * @file expander-get-gpio.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.2
 * @date 2022-06-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <expander_i2c.h>



int main(int argc, char *argv[]){

// on affiche une aide si la personne ne met pas d'argument  

    if(argc != 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1],"--help")){
    
        printf("Usage: ./get-expander-gpio <addresse> \n");
        printf("exemple: ./get_expander_gpio 26 \n");
        exit(EXIT_FAILURE);

    }

// variable qui contiendra l'adresse l'expander en question
     uint8_t addr;

// on cherche quelle adresse il a saisit pour pouvoir la donner a la variable addr
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
//on renvoit un message si ca ne fait pas parti des adresses possibles de l'expander
    else{

        printf("fonction %s : addresse doit etre entre 20 et 27\n", __func__);

        printf("Usage: ./get-expander-gpio <addresse> \n");
        printf("exemple: ./get_expander_gpio 26 \n");
        exit(EXIT_FAILURE);
    }

// on initialise un instance
    expander_t *e = expander_init(addr);

// on affiche les gpio sur la console
    expander_printGPIO(e);

// on libère l'instance
    expander_closeAndFree(e);
    
// on sort
    return EXIT_SUCCESS;
}
