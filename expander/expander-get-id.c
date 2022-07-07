/**
 * @file expander-get-gpio.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.3
 * @date 2022-06-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <rtc_eeprom.h>



int main(int argc, char* argv[]) 
{

    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();
    char* str_id;

        long id = 0;
        long id2 = 0;

        for (size_t i = 0; i < 3; i++)
        {
            /* code */
            id = id + (eeprom_readProtected(rtc_eeprom, 0xF2 + i) << (8*i));
            printf("id: %X \n", id);
        }
            for (size_t i = 0; i < 3; i++)
        {
            /* code */
            id2 = id2 + (eeprom_readProtected(rtc_eeprom, 0xF5 + i) << (8*i));
            //printf("id2: %X \n", id2);
        }
        
        
        char str_id1[7];
        char str_id2[7];


        
        sprintf(str_id1, "%.06X", id);
        sprintf(str_id2, "%.06X", id2);
        strcat(str_id,str_id2);
        strcat(str_id,str_id1);
        

        
        rtc_eeprom_closeAndFree(rtc_eeprom);
}