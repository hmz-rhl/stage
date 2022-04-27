#include <stdio.h>
//#include "paho.h" librairie C pour mqtt à installer https://github.com/eclipse/paho.mqtt.c


/**
 * 
 * sub et afficher la valeur lu sur le topic :      up/value/temp
 * 
 **/

 #include <stdio.h>
#include <stdlib.h>

#include <mosquitto.h>





int main() {
	
    mosquitto_connect("up/value/temp", "127.0.0.1", 1883, 30);
    


	return 0;
}



