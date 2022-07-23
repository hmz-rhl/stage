/**
 * @file hubload.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.4
 * @date 2022-05-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <mosquitto.h> // Broker MQTT
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <wiringPi.h> // compilation ajouter -lwiringPi
#include <pthread.h> // compilation ajouter -lptrhread
// #include <softPwm.h>

// #include <bcm2835.h>


#include <pn532.h>
#include <PN532_Rpi_I2C.h>
#include <rtc_eeprom.h>
#include <expander_i2c.h>

#include <MCP3202.h>

#include <clk.h>
#include <gpio.h>
#include <dma.h>
#include <pwm.h>


#include <ws2811.h>


//Pins Wpi
#define LOCK_P 21
#define CP_PWM 23
#define I2C_D 8 // Pin de data I2C
#define I2C_C 9 // Pin de clock I2C
#define SM_TIC_D 2
#define USER_KEY 3
#define MOSI 12 // Pin du MOSI SPI
#define MISO 13 // Pin du MISO SPI
#define SCLK 14
#define PP_IN 22
#define WD_TRIP 24
#define CF4 25
#define RCD_TRIP_DC 1
#define RCD_TRIP_AC 4
#define IRQ1 5
#define IRQ0 6
#define LOCK_FB 10
#define LED_STRIP_D 29
#define SCREEN_PWM 26

#define MONO	1
#define TRI		3

#define NB_LED 112

#define RGB_BLINK 1
#define GREEN_BLINK 2
#define RED_BLINK 3
#define BLUE_BLINK 4
#define RAINBOW_CIRCLE 5
#define RED_CHENILLE 6
#define GREEN_CHENILLE 7
#define BLUE_CHENILLE 8
#define ORANGE_BLINK 9


struct mosquitto *mosq;
int dutycycle;
uint8_t scan_activated = 0;
int user_key_clicked = 0;
unsigned long long compteur_tic = 0;
unsigned long long historique_Wh = 0;
struct timeval start;
struct timeval end;
double power = 0;
double current = 0;
int charge_active = 0;
int mode_phase = MONO; // 1 -> tri | 0 -> Mono
int tempo = 0;
int cp_old = -1;
int cp_old2 = 0;

int pp_old = -1;

int time_up = 1000;
int time_low = 0;
double cp_cpt = 0;
double cp_tot = 0;
double pp_cpt = 0;
double pp_tot = 0;
int modeS0 = 1;
int cpt_csv = 0;
int csv_activated = 0;
int s0_activated = 0;
uint32_t mainled = 0xF0F000;
int mode_led = RED_CHENILLE;
int cp_activated = 0 ;
int plugged = 0;
int old_pwm = 0;
int type2_closed = 0;

ws2811_t ledstring;

// sleep plus precis en seconde
void Sleep(uint time) {

	usleep(1000000*time);
}

/**
 ** 
 * @brief  permet d'obtenir l'ID de la borne stocker dans l'eeprom 
 * 
 * @param   str_id chaine de caractere dans laquelle sera stocker l'ID
 * 
 * 
 * @return  ne retourne rien 
 *  
 **/
void eeprom_getStringID(char* str_id)
{

	rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();

	long id = 0;
    long id2 = 0;

	for (size_t i = 0; i < 3; i++)
	{
		/* code */
		id = id + (eeprom_readProtected(rtc_eeprom, 0xF2 + i) << (8*i));
        //printf("id: %X \n", id);
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

/**
 ** 
 * @brief  permet d'ecrire un ID dans l'eeprom
 * 
 * @param  id ID a ecrire 
 * 
 * 
 * @return  ne retourne rien
 *  
 **/
void eeprom_writeID(char *id){


	uint8_t a,b;
	if(strlen(id) != 12)
	{
		printf("Error %s: Id n'est pas de la taille autorisé (12 caractères 0-F)\n", __func__);
		return;
	}

// on verifie l'id, il doit etre dans le format suivant : ABCDEF012345
	for(int i = 0; i<12 ; i++){

		if((id[i] >= 48 && id[i]<=57) || (id[i] >= 'A' && id[i] < 'Z'))
		{
			
		}
		else
		{
		    printf("Error %s: ID incorrect, les caracteres utilisés ne sont pas corrects (0-F)\n", __func__);
		    return;
		}
	}

	rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();

// on reset le compteur d'energie
	eeprom_writeProtected(rtc_eeprom, 0xF0, 0x00);
	eeprom_writeProtected(rtc_eeprom, 0xF1, 0x00);

	char id2[15] = "0x";
    strcat(id2,id);
    long value = strtol( id2,NULL, 16 );
	// on ecrit l'id
	for (size_t i = 0; i < 6; i++)
	{
		/* code */
		eeprom_writeProtected(rtc_eeprom, 0xF2 + i, (value >> 8*i) & 0xFF);
	}

	eeprom_printProtected(rtc_eeprom);
	
	rtc_eeprom_closeAndFree(rtc_eeprom);

}

/**
 ** 
 * @brief  permet de lire l'energie cumulée en Wh dans les deux premiers registres de l'eeprom protégée
 * 
 * @param  
 * 
 * 
 * @return  retourne la valeur de l'énergie cumulée 
 *  
 **/
uint16_t eeprom_getWh()
{
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();
    uint8_t val1 = eeprom_readProtected (rtc_eeprom, 0xF0);
    uint8_t val2 = eeprom_readProtected (rtc_eeprom, 0xF1);
    uint16_t result = ((val2 << 8) | val1);

	rtc_eeprom_closeAndFree(rtc_eeprom);
    return result;
}

/**
 ** 
 * @brief  fonction d'interruption du S0, a chaque tic cette fonctionest appelé afin de modifier l'eeprom protegee pour stocker l'energie cumulee
 * 
 * @param  
 * 
 * 
 * @return  ne retourne rien
 *  
 **/
void S0_interrupt(void){ 

    gettimeofday(&end, NULL);
    rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();
    // time_t temps;
    double temps = (end.tv_sec - start.tv_sec) + 1e-6*(end.tv_usec - start.tv_usec);

    //eeprom_printProtected(rtc_eeprom);
    

    //ecriture de la charge cumulée dans l'eeprom protegee

    if(eeprom_readProtected(rtc_eeprom,0xF0) == 0xFF)
    {
        eeprom_writeProtected(rtc_eeprom, 0xF0, 0x00);
       
        if(eeprom_readProtected(rtc_eeprom,0xF1) == 0xFF)
        {
            eeprom_writeProtected(rtc_eeprom, 0xF1, 0x00);
        }
        else
        {
            uint8_t val_F1 = eeprom_readProtected(rtc_eeprom,0xF1);
            eeprom_writeProtected(rtc_eeprom, 0xF1, val_F1 + 1);
        }

    }
    
    else
    {
       uint8_t val_F0 = eeprom_readProtected(rtc_eeprom,0xF0);
       eeprom_writeProtected(rtc_eeprom, 0xF0, val_F0 + 1);
    }

    rtc_eeprom_closeAndFree(rtc_eeprom);

	if(mode_phase == MONO){

		power = 1.0/(temps/3600.0);
		current = power/230.0;
	}
	else{
		//TODO
	}


    start = end;

}

/**
 ** 
 * @brief  publie via MQTT l'etat du bouton 
 * 
 * @param  
 * 
 * 
 * @return ne retourne rien 
 *  
 **/
void user_key_interrupt(void){

	//printf("%s: Début de routine\n",__func__);
	int rc;
	char str[] = "1";
	if(digitalRead(USER_KEY) == 1){

		rc = mosquitto_publish(mosq, NULL, "up/btn/released", strlen("released"), "released", 2, false);
		if(rc != MOSQ_ERR_SUCCESS){
			fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
		}
	}
	else{

		rc = mosquitto_publish(mosq, NULL, "up/btn/pressed", strlen("pressed"), "pressed", 2, false);
		if(rc != MOSQ_ERR_SUCCESS){
			fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
		}
	}
	//printf("%s: Fin de routine\n",__func__);
}

/**
 ** 
 * @brief  fonction principale du thread rfid 
 * 
 * @param  ptr pointeur sur arguments a donner au thread(pas utilisé)
 * 
 * 
 * @return ne retourne rien
 *  
 **/
void *thread_rfid(void *ptr)
{
	if(wiringPiSetup() < 0)
	{
		fprintf(stderr, "fonction %s: Unable to set up: %s\n", __func__, strerror(errno));
		exit(EXIT_FAILURE);
	}
	uint8_t buff[255];
    uint8_t uid[MIFARE_UID_MAX_LENGTH];
	char *str[MIFARE_UID_MAX_LENGTH];

    uint32_t pn532_error = PN532_ERROR_NONE;
    int32_t uid_len = 0;
    printf("Hello je suis le thread RFID !\r\n");
    PN532 pn532;


        

	for(;;){

		if(scan_activated){
			
			Sleep(1);

			while(PN532_I2C_Init(&pn532) < 0 && scan_activated){

				Sleep(1);
			}
			printf("PN532 initialised \n");
   			while(PN532_GetFirmwareVersion(&pn532, buff) != PN532_STATUS_OK) {
		
				Sleep(1);
				if(scan_activated == 0){
					PN532_I2C_Close();
					break;
				}
    		}

			printf("Found PN532 with firmware version: %d.%d\r\n", buff[1], buff[2]);
    		PN532_SamConfiguration(&pn532);

			printf("Waiting for RFID/NFC card...\r\n");
			while(scan_activated)
			{
				if(scan_activated == 0){
					PN532_I2C_Close();
					break;
				}
				// Check if a card is available to read
				uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 1000);
				if (uid_len == PN532_STATUS_ERROR) 
				{
					fflush(stdout);
				} 
				else 
				{

					printf("\n Found card with UID: ");
			
					char message[256];
					printf("%02x%02x%02x%02x%02x%02x%02x%02x \n",uid[0],uid[1],uid[2],uid[3],uid[4],uid[5],uid[6],uid[7]);
					
					sprintf(message, "%02x%02x%02x%02x%02x%02x%02x%02x", uid[0],uid[1],uid[2],uid[3],uid[4],uid[5],uid[6],uid[7]);
					mosquitto_publish(mosq,NULL,"up/scan",strlen(message),message,2,false);
					printf("\r\n");
								
				}
				Sleep(1);

			}
		
		}
		else{
			Sleep(1);
		}


	}
}


/**
 ** 
 * @brief  fonction principale du thread led 
 * 
 * @param  ptr pointeur sur arguments a donner au thread(pas utilisé)
 * 
 * 
 * @return ne retourne rien
 *  
 **/
void *thread_led(void *ptr){

	ledstring.freq = WS2811_TARGET_FREQ;
	ledstring.dmanum = 10;

	ledstring.channel[0].gpionum = 21;
	ledstring.channel[0].invert = 0;
	ledstring.channel[0].count = NB_LED;
	ledstring.channel[0].strip_type = WS2811_STRIP_GBR;
	ledstring.channel[0].brightness = 127;
//inutile mais on definit
	ledstring.channel[1].gpionum = 0;
	ledstring.channel[1].invert = 0;
	ledstring.channel[1].count = 0;
	ledstring.channel[1].brightness = 0;
	ws2811_return_t ret;



   	while ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
        sleep(1);
    }

    while (1)
    {
		if(mode_led == RGB_BLINK){

			for(int j = 0; j < 3; j++ ) {
		// Fade IN
				for(int k = 0; k < 256; k++) {

					
					for (size_t i = 0; i < NB_LED; i++)
					{
						/* code */
						ledstring.channel[0].leds[i] = 0x000000 + (k<<(16-8*j));
					}
					

					if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
					{
						fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
						break;
					}
					//delay(3);
					usleep(3000);
				}	
				// Fade OUT
				for(int k = 255; k >= 0; k--) {
				
					for (size_t i = 0; i < NB_LED; i++)
					{
						/* code */
						ledstring.channel[0].leds[i] = 0x000000 + (k<<(16-8*j));
					}
					

					if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
					{
						fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
						break;
					}
					//delay(3);
					usleep(3000);
				}
			}	
		}
		

		else if(mode_led == RAINBOW_CIRCLE){
			unsigned char c[3];
			unsigned char WheelPos;
			uint16_t i, j;

			for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel

				for(i=0; i< NB_LED; i++) {
					

					WheelPos = (((i * 256 / NB_LED) + j) & 255);
					if(WheelPos < 85) {
						c[0]=WheelPos * 3;
						c[1]=255 - WheelPos * 3;
						c[2]=0;
					} else if(WheelPos < 170) {
						WheelPos -= 85;
						c[0]=255 - WheelPos * 3;
						c[1]=0;
						c[2]=WheelPos * 3;
					} else {
						WheelPos -= 170;
						c[0]=0;
						c[1]=WheelPos * 3;
						c[2]=255 - WheelPos * 3;
					}
					ledstring.channel[0].leds[i] = (c[2]<<16) + (c[1]<<8) + c[0];
					if(mode_led != RAINBOW_CIRCLE){
						j= 256*5;
						i= NB_LED;
						
					}
				}
				if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
				{
					fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
					break;
				}
				

				usleep(10000);
				
			}
					

		}

		else if(mode_led == GREEN_BLINK){

			
			// Fade IN
			for(int k = 0; k < 256; k++) {

				
				for (size_t i = 0; i < NB_LED; i++)
				{
					/* code */
					ledstring.channel[0].leds[i] = 0x000000 + (k<<8);
				}
				

				if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
				{
					fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
					break;
				}
				//delay(3);
				usleep(3000);
			}	
			// Fade OUT
			for(int k = 255; k >= 0; k--) {
			
				for (size_t i = 0; i < NB_LED; i++)
				{
					/* code */
					ledstring.channel[0].leds[i] = 0x000000 + (k<<8);
				}
				

				if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
				{
					fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
					break;
				}
				//delay(3);
				usleep(3000);
			}
				
		}

		else if(mode_led == BLUE_BLINK){
						for(int k = 0; k < 256; k++) {

				
				for (size_t i = 0; i < NB_LED; i++)
				{
					/* code */
					ledstring.channel[0].leds[i] = 0x000000 + (k<<16);
				}
				

				if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
				{
					fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
					break;
				}
				//delay(3);
				usleep(3000);
			}	
			// Fade OUT
			for(int k = 255; k >= 0; k--) {
			
				for (size_t i = 0; i < NB_LED; i++)
				{
					/* code */
					ledstring.channel[0].leds[i] = 0x000000 + (k<<16);
				}
				

				if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
				{
					fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
					break;
				}
				//delay(3);
				usleep(3000);
			}
		}

		else if(mode_led == RED_BLINK){
						for(int k = 0; k < 256; k++) {

				
				for (size_t i = 0; i < NB_LED; i++)
				{
					/* code */
					ledstring.channel[0].leds[i] = 0x000000 + (k);
				}
				

				if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
				{
					fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
					break;
				}
				//delay(3);
				usleep(3000);
			}	
			// Fade OUT
			for(int k = 255; k >= 0; k--) {
			
				for (size_t i = 0; i < NB_LED; i++)
				{
					/* code */
					ledstring.channel[0].leds[i] = 0x000000 + (k);
				}
				

				if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
				{
					fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
					break;
				}
				//delay(3);
				usleep(3000);
			}
		}
		
		else if(mode_led == RED_CHENILLE){

			for(int i = 0; i < NB_LED-1-2; i++) {
				for (size_t i = 0; i < NB_LED; i++)
				{
					/* code */
					ledstring.channel[0].leds[i] = 0;
				}
				ledstring.channel[0].leds[i] = 0x20;
				for(int j = 1; j <= 1; j++) {
					ledstring.channel[0].leds[i+j] = 0xFF;
				}
				ledstring.channel[0].leds[i+1+1] = 0x20;
				if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
				{
					fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
					break;
				}
				usleep(30000);
 			}
		}

		else if(mode_led == GREEN_CHENILLE){

			for(int i = 0; i < NB_LED-1-2; i++) {
				for (size_t i = 0; i < NB_LED; i++)
				{
					/* code */
					ledstring.channel[0].leds[i] = 0;
				}
				ledstring.channel[0].leds[i] = 0x2000;
				for(int j = 1; j <= 1; j++) {
					ledstring.channel[0].leds[i+j] = 0xFF00;
				}
				ledstring.channel[0].leds[i+1+1] = 0x2000;
				if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
				{
					fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
					break;
				}
				usleep(30000);
 			}
		}
		else if(mode_led == BLUE_CHENILLE){

			for(int i = 0; i < NB_LED-1-2; i++) {
				for (size_t i = 0; i < NB_LED; i++)
				{
					/* code */
					ledstring.channel[0].leds[i] = 0;
				}
				ledstring.channel[0].leds[i] = 0x200000;
				for(int j = 1; j <= 1; j++) {
					ledstring.channel[0].leds[i+j] = 0xFF0000;
				}
				ledstring.channel[0].leds[i+1+1] = 0x200000;
				if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
				{
					fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
					break;
				}
				usleep(30000);
 			}
		}
		else{

			for (size_t i = 0; i < NB_LED; i++)
			{
				/* code */
				ledstring.channel[0].leds[i] = mainled;
			}

			if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
			{
				fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
				break;
			}
			usleep(30000);
		}
    }

	ws2811_fini(&ledstring);
    

}
/**
 ** 
 * @brief  fonction a executer lors d'une interruption par ctrl+C qui permet de detruire l'instance mosquitto
 * 
 * @param  n
 * 
 * 
 * @return  ne retourne rien 
 *  
 **/
void nettoyage(int n)
{
	
	printf("%s: interruption on detruit l'instance mosq\n", __func__);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	if(&ledstring != NULL)	ws2811_fini(&ledstring);
	
	exit(EXIT_SUCCESS);
}


/**
 ** 
 * @brief  fonction qui passe de volt a degre pour le composant TMP36
 * 
 * @param  tension tension a convertir
 * 
 * 
 * @return ne retourne rien 
 *  
 **/
double toDegres(int tension){

	if(tension < 0){

		printf("%s: ne peut pas convertir une tension negative en degres", __func__);
		return tension;
	}
	return (toMillivolt(tension)-500)/10;
}


/* Callback called when the client receives a CONNACK message from the broker. */
void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	int rc;
	/* Print out the connection result. mosquitto_connack_string() produces an
	 * appropriate string for MQTT v3.x clients, the equivalent for MQTT v5.0
	 * clients is mosquitto_reason_string().
	 */
	printf("%s: %s\n",__func__ ,mosquitto_connack_string(reason_code));
	if(reason_code != 0){
		/* If the connection fails for any reason, we don't want to keep on
		 * retrying in this example, so disconnect. Without this, the client
		 * will attempt to reconnect. */
		mosquitto_disconnect(mosq);
	}

	/* Making subscriptions in the on_connect() callback means that if the
	 * connection drops and is automatically resumed by the client, then the
	 * subscriptions will be recreated when the client reconnects. */
	//rc = mosquitto_subscribe(mosq, NULL, "example/temperature", 1);
    char *topics[22]= {"down/type_ef/open","down/type_ef/close","down/type2/open","down/type2/close","down/charger/pwm","down/lockType2/open","down/lockType2/close","down/scan/activate","down/scan/shutdown", "down/ID/write", "down/ID/read", "down/lock_vae/open", "down/lock_vae/close", "down/power_vae/open", "down/power_vae/close","down/charger/phases","down/csv/start","down/s0/activate", "down/s0/shutdown","down/main_led","down/cp/activate","down/cp/shutdown"};

    rc = mosquitto_subscribe_multiple(mosq,NULL,22,topics,2,0,NULL);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
		/* We might as well disconnect if we were unable to subscribe */
		mosquitto_disconnect(mosq);
	}
}


/* Callback called when the broker sends a SUBACK in response to a SUBSCRIBE. */
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
	int i;
	bool have_subscription = false;

	/* In this example we only subscribe to a single topic at once, but a
	 * SUBSCRIBE can contain many topics at once, so this is one way to check
	 * them all. */
	for(i=0; i<qos_count; i++){
		printf("%s: %d:granted qos = %d\n",__func__, i, granted_qos[i]);
		if(granted_qos[i] <= 2){
			have_subscription = true;
		}
	}
	if(have_subscription == false){
		/* The broker rejected all of our subscriptions, we know we only sent
		 * the one SUBSCRIBE, so there is no point remaining connected. */
		fprintf(stderr, "Error: All subscriptions rejected.\n");
		mosquitto_disconnect(mosq);
		exit(EXIT_FAILURE);
	}
}



/* Callback called when the client knows to the best of its abilities that a
 * PUBLISH has been successfully sent. For QoS 0 this means the message has
 * been completely written to the operating system. For QoS 1 this means we
 * have received a PUBACK from the broker. For QoS 2 this means we have
 * received a PUBCOMP from the broker. */
void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	//printf("%s: Message %d has been published.\n\n",__func__, mid);
}


/* Callback called when the client receives a message. */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	/* This blindly prints the payload, but the payload can be anything so take care. */
	printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
    

    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);

    if(!strcmp(msg->topic,"down/type_ef/open")){

        expander_t* expander = expander_init(0x26); //Pour les relais
        expander_resetPinGPIO(expander, TYPE_E_F_ON); 
        printf("Le relais de la prise E/F est ouvert\n");
        expander_closeAndFree(expander);
		current = 0;
		power = 0;
    }

    if(!strcmp(msg->topic,"down/type_ef/close")){
        expander_t* expander = expander_init(0x26); //Pour les relais
        expander_setPinGPIO(expander, TYPE_E_F_ON);
        printf("Le relais de la prise E/F est ferme\n");
        expander_closeAndFree(expander);
    }

    if(!strcmp(msg->topic,"down/type2/close")){

        // if(!strcmp(msg->payload, "1")){
        //     expander_t* expander = expander_init(0x26); //Pour les relais
        //     expander_setPinGPIO(expander,TYPE_2_NL1_ON);
        //     expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);

        //     printf("Les relais N et L1 de la prise type 2 sont fermes\n");
        //     expander_closeAndFree(expander);
        // }
        // else if(!strcmp(msg->payload, "3")){
            expander_t* expander = expander_init(0x26); //Pour les relais
            expander_setPinGPIO(expander,TYPE_2_NL1_ON);
            expander_setPinGPIO(expander, TYPE_2_L2L3_ON);

            printf("Les relais N, L2 et L3 de la prise type 2 sont fermes\n");
            expander_closeAndFree(expander);
			type2_closed = 1;

        // }

    }

    if(!strcmp(msg->topic,"down/type2/open")){

        // if(!strcmp(msg->payload, "1")){
        //     expander_t* expander = expander_init(0x26); //Pour les relais
        //     expander_resetPinGPIO(expander, TYPE_2_NL1_ON);
        //     // expander_resetPinGPIO(expander, 1);
        //     printf("Les relais N et L1 de la prise type 2 sont ouverts\n");
        //     expander_closeAndFree(expander);
		// 	current = 0;
		// 	power = 0;
        // }
        // else if(!strcmp(msg->payload, "3")):{
            expander_t* expander = expander_init(0x26); //Pour les relais
            expander_resetPinGPIO(expander, TYPE_2_NL1_ON);
            expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);
            printf("Les relais L2 et L3 de la prise type 2 sont ouvert\n");
            expander_closeAndFree(expander);
			type2_closed = 0;
        // }
    
    }

    if(!strcmp(msg->topic, "down/charger/pwm")){

        

        if(atoi(msg->payload) >= 0 && atoi(msg->payload) <= 100){
            
        	dutycycle = atoi(msg->payload);
            printf("dutycycle %d \n",dutycycle);
			//pwmWrite(CP_PWM, dutycycle);
			
        }
		

    }
    
    if(!strcmp(msg->topic,"down/lockType2/close")){

		expander_t* expander = expander_init(0x26); //Pour les relais
		

		expander_setPinGPIO(expander, LOCK_D);
		usleep(1);
		digitalWrite(LOCK_P,1);

		sleep(1);
		digitalWrite(LOCK_P,0);


		if(strcmp("force",msg->payload) && digitalRead(LOCK_FB) != 0){
			expander_resetPinGPIO(expander, LOCK_D);
			usleep(1);

			digitalWrite(LOCK_P,1);
			Sleep(1);
			digitalWrite(LOCK_P,0);

			if(digitalRead(LOCK_FB) != 0){
				
				printf("Error %s: feedback non recu pour la fermeture de la prise\n",__func__);
				int rc = mosquitto_publish(mosq, NULL, "up/logs", strlen("feedback non recu pour la fermeture de la prise"), "feedback non recu pour la fermeture de la prise", 2, false);
				if(rc != MOSQ_ERR_SUCCESS){
					fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
				}

			}
			else{
				printf("Le moteur est ferme\n");
			}
		}
        else{
			printf("Le moteur est ferme\n");
		}
		expander_closeAndFree(expander);
	}
    if(!strcmp(msg->topic,"down/lockType2/open")){
		expander_t* expander = expander_init(0x26); //Pour les relais
		expander_resetPinGPIO(expander, LOCK_D);
		usleep(1);

		digitalWrite(LOCK_P,1);

		Sleep(1);
		digitalWrite(LOCK_P,0);

		if(strcmp("force",msg->payload) && digitalRead(LOCK_FB) != 1){

			expander_setPinGPIO(expander, LOCK_D);
			usleep(1);

			digitalWrite(LOCK_P,1);
			Sleep(1);
			digitalWrite(LOCK_P,0);
			if(digitalRead(LOCK_FB) != 1){
				printf("Error %s: feedback non recu pour l'ouverture de la prise\n",__func__);
				int rc = mosquitto_publish(mosq, NULL, "up/logs", strlen("feedback non recu pour l'ouverture de la prise"), "feedback non recu pour l'ouverture de la prise", 2, false);
				if(rc != MOSQ_ERR_SUCCESS){
					fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
				}

			}
			else{
				printf("Le moteur est ouvert\n");
			}
		}
        else{
			printf("Le moteur est ouvert\n");
		}

		expander_closeAndFree(expander);
		
    }

	if( !strcmp(msg->topic, "down/scan/activate"))
	{
		scan_activated = 1;
	}

	if( !strcmp(msg->topic, "down/scan/shutdown"))
	{
		scan_activated = 0;
	}

	if(!strcmp(msg->topic, "down/ID/write")){
		eeprom_writeID(msg->payload);
	}

	if(!strcmp(msg->topic, "down/ID/read")){
		
		char ID[100];
		// eeprom_getStringID(ID);
		rtc_eeprom_t *rtc_eeprom = rtc_eeprom_init();

		long id = 0;
		long id2 = 0;

		for (size_t i = 0; i < 3; i++)
		{
			/* code */
			id = id + (eeprom_readProtected(rtc_eeprom, 0xF2 + i) << (8*i));
			//printf("id: %X \n", id);
		}
			for (size_t i = 0; i < 3; i++)
		{
			/* code */
			id2 = id2 + (eeprom_readProtected(rtc_eeprom, 0xF5 + i) << (8*i));
			//printf("id2: %X \n", id2);
		}
		
		
		char str_id1[4];
		char str_id2[4];


		
		sprintf(str_id1, "%.06X", id);
		sprintf(str_id2, "%.06X", id2);
		strcat(ID,str_id2);
		strcat(ID,str_id1);
		//correction des 3 valeurs chelou :/
		strcpy(ID,ID + 3);
		rtc_eeprom_closeAndFree(rtc_eeprom);
		int rc = mosquitto_publish(mosq, NULL, "up/ID", strlen(ID), ID, 2, false);
		if(rc != MOSQ_ERR_SUCCESS){
			fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
		}
	}
	if(!strcmp(msg->topic,"down/lock_vae/open")){

        if(!strcmp(msg->payload, "1")){

            expander_t* expander = expander_init(0x20); //Pour les relais
            expander_resetPinGPIO(expander, 0);
            printf("Le relais du velo 1 est ouvert\n");
            expander_closeAndFree(expander);
			
        }
		else if(!strcmp(msg->payload, "2")){

            expander_t* expander = expander_init(0x20); //Pour les relais
            expander_resetPinGPIO(expander, 1);
            printf("Le relais du velo 2 est ouvert\n");
            expander_closeAndFree(expander);
			
        }
		else if(!strcmp(msg->payload, "3")){

            expander_t* expander = expander_init(0x20); //Pour les relais
            expander_resetPinGPIO(expander, 2);
            printf("Le relais du velo 3 est ouvert\n");
            expander_closeAndFree(expander);
			
        }
		else if(!strcmp(msg->payload, "4")){

            expander_t* expander = expander_init(0x20); //Pour les relais
            expander_resetPinGPIO(expander, 3);
            printf("Le relais du velo 4 est ouvert\n");
            expander_closeAndFree(expander);
			
        }
        
    
    }

	if(!strcmp(msg->topic,"down/lock_vae/close")){

		if(!strcmp(msg->payload, "1")){

			expander_t* expander = expander_init(0x20); //Pour les relais
			expander_setPinGPIO(expander, 0);
			printf("Le relais du velo 1 est ferme\n");
			expander_closeAndFree(expander);
			
		}
		else if(!strcmp(msg->payload, "2")){

			expander_t* expander = expander_init(0x20); //Pour les relais
			expander_setPinGPIO(expander, 1);
			printf("Le relais du velo 2 est ferme\n");
			expander_closeAndFree(expander);
			
		}
		else if(!strcmp(msg->payload, "3")){

			expander_t* expander = expander_init(0x20); //Pour les relais
			expander_setPinGPIO(expander, 2);
			printf("Le relais du velo 3 est ferme\n");
			expander_closeAndFree(expander);
			
		}
		else if(!strcmp(msg->payload, "4")){

			expander_t* expander = expander_init(0x20); //Pour les relais
			expander_setPinGPIO(expander, 3);
			printf("Le relais du velo 4 est ferme\n");
			expander_closeAndFree(expander);
			
		}
        
    
    }

	if(!strcmp(msg->topic,"down/power_vae/open")){

        if(!strcmp(msg->payload, "1")){

            expander_t* expander = expander_init(0x20); //Pour les relais
            expander_resetPinGPIO(expander, 4);
            printf("La prise du velo 1 est ouvert\n");
            expander_closeAndFree(expander);
			
        }
		else if(!strcmp(msg->payload, "2")){

            expander_t* expander = expander_init(0x20); //Pour les relais
            expander_resetPinGPIO(expander, 5);
            printf("La prise du velo 2 est ouvert\n");
            expander_closeAndFree(expander);
			
        }
		else if(!strcmp(msg->payload, "3")){

            expander_t* expander = expander_init(0x20); //Pour les relais
            expander_resetPinGPIO(expander, 6);
            printf("La prise du velo 3 est ouvert\n");
            expander_closeAndFree(expander);
			
        }
		else if(!strcmp(msg->payload, "4")){

            expander_t* expander = expander_init(0x20); //Pour les relais
            expander_resetPinGPIO(expander, 7);
            printf("La prise du velo 4 est ouvert\n");
            expander_closeAndFree(expander);
			
        }
        
    
    }

	if(!strcmp(msg->topic,"down/power_vae/close")){

		if(!strcmp(msg->payload, "1")){

			expander_t* expander = expander_init(0x20); //Pour les relais
			expander_setPinGPIO(expander, 4);
			printf("La prise du velo 1 est ferme\n");
			expander_closeAndFree(expander);
			
		}
		else if(!strcmp(msg->payload, "2")){

			expander_t* expander = expander_init(0x20); //Pour les relais
			expander_setPinGPIO(expander, 5);
			printf("La prise du velo 2 est ferme\n");
			expander_closeAndFree(expander);
			
		}
		else if(!strcmp(msg->payload, "3")){

			expander_t* expander = expander_init(0x20); //Pour les relais
			expander_setPinGPIO(expander, 6);
			printf("La prise du velo 3 est ferme\n");
			expander_closeAndFree(expander);
			
		}
		else if(!strcmp(msg->payload, "4")){

			expander_t* expander = expander_init(0x20); //Pour les relais
			expander_setPinGPIO(expander, 7);
			printf("La prise du velo 4 est ferme\n");
			expander_closeAndFree(expander);
			
		}
        
    
    }
	if(!strcmp(msg->topic,"down/charger/phases")){

		if(!strcmp(msg->payload, "1")){

			mode_phase = MONO;
			
		}
		else if(!strcmp(msg->payload, "3")){

			mode_phase = TRI;
			
		}
	}

	if(!strcmp(msg->topic,"down/csv/start")){

		csv_activated = 1;
	}

	if(!strcmp(msg->topic,"down/s0/activate")){

		s0_activated = 1;
	}

	if(!strcmp(msg->topic,"down/s0/shutdown")){

		s0_activated = 0;
	}

	if(!strcmp(msg->topic,"down/cp/activate")){

		cp_activated = 1;
	}

	if(!strcmp(msg->topic,"down/cp/shutdown")){

		cp_activated = 0;
	}

	if(!strcmp(msg->topic,"down/main_led")){

		if(!strcmp(msg->payload,"RED_BLINK"))
		{
			mode_led = RED_BLINK;
		}
		else if(!strcmp(msg->payload,"GREEN_BLINK")){

			mode_led = GREEN_BLINK;
		}
		else if(!strcmp(msg->payload,"BLUE_BLINK")){

			mode_led = BLUE_BLINK;
		}
		else if(!strcmp(msg->payload,"RAINBOW_CIRCLE")){

			mode_led = RAINBOW_CIRCLE;
		}
		else if(!strcmp(msg->payload,"RGB_BLINK")){

			mode_led = RGB_BLINK;
		}
		else if(!strcmp(msg->payload,"RED_CHENILLE")){

			mode_led = RED_CHENILLE;
		}
		else if(!strcmp(msg->payload,"GREEN_CHENILLE")){

			mode_led = GREEN_CHENILLE;
		}
		else if(!strcmp(msg->payload,"BLUE_CHENILLE")){

			mode_led = BLUE_CHENILLE;
		}
		else{

			mainled = (uint32_t)strtol((msg->payload), NULL, 16);
			mode_led = 0;
			for(int i = 0; i<51 ; i++){
				// inversement de RGB -> BGR pour les fonctions du ws281x
				ledstring.channel[0].leds[i] = (mainled & 0x00FF00) + ((mainled & 0x0000FF) << 16) + ((mainled & 0xFF0000) >> 16);
			}
		}
	}

}

// fonction qui lit et publie les valeurs en mqtt
void publish_values(struct mosquitto *mosq)
{

	/* Publish the message
	 * mosq - our client instance
	 * *mid = NULL - we don't want to know what the message id for this message is
	 * topic = "example/temperature" - the topic on which this message will be published
	 * payloadlen = strlen(payload) - the length of our payload in bytes
	 * payload - the actual payload
	 * qos = 2 - publish with QoS 2 for this example
	 * retain = false - do not use the retained message feature for this message
	 */

	char payload[20];
	double pp, cp, temp, cp_reel;
	int PP, CP;
	
	
	// variable pour debug mqtt
	int rc;
	char str_temp[100], str_cp[100],  str_pp[100];

    cp = toVolt(readAdc(0,CP_CS));
	// printf("brute adc_CP: %lfV\n", cp);
	cp_reel = 4.0*cp;
	cp_tot += cp_reel;
	cp_cpt++;
	//printf("cp reel : %lf\n",cp_reel);

	

	if(cp_cpt>=10){

	// on donne a CP les vraies valeurs correspondantes 
	//	printf("cpt %lf \ncp_reel %lf\n", cp_cpt, cp_reel);
		cp_reel = cp_tot/(double)cp_cpt;
	//	printf("cpt %lf \ncp_reel %lf\n", cp_cpt, cp_reel);


		cp_cpt = 0;
		cp_tot = 0;

		CP = -12;
		if (cp_reel > 10.5){

			CP = 12;
		}
		else if( cp_reel >= 7.5){

			CP = 9;
		}
		else if( cp_reel >= 4.5){

			CP = 6;
		}
		else if( cp_reel >= 1.5){

			CP = 3;
		}
		else if( cp_reel > -1.5){

			CP = 0;
		}
		else{

			CP = -12;
		}
		//printf("-->CP: %d\n", CP);
		
		if(CP != cp_old || tempo>300){

			//printf("brute CP: %d\n", CP);
			sprintf(str_cp, "%d", CP);
			rc = mosquitto_publish(mosq, NULL, "up/value/cp", strlen(str_cp), str_cp, 2, false);
			if(rc != MOSQ_ERR_SUCCESS){
				fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
			}
			if(CP != cp_old){

				cp_old2 = cp_old;
				cp_old = CP;

			}
		}
	}
// on stringify ce qu'il faut publier
// affiche sur la console

	


	pp = toVolt(readAdc(0,PP_CS));
	pp_tot += pp;
	pp_cpt++;
	// printf("brute adc_PP: %lfV\n", pp);

// on donne a PP les valeurs correspondantes 
	if(pp_cpt >= 10){
		
		pp = pp_tot/(double)pp_cpt;
		pp_tot = 0;
		pp_cpt = 0;

		if (pp < 0.58){

			PP = 80;
			plugged = 1;
		}
		else if( pp < 0.9 ){

			PP = 63;
			plugged = 1;
		}
		else if( pp < 1.5 ){

			PP = 32;
			plugged = 1;
		}
		else if( pp < 2.2 ){

			PP = 20;
			plugged = 1;
		}
		else if( pp < 2.6 ){

			PP = 13;
			plugged = 1;
		}
		else{
			
			PP = 6;
			plugged = 0;
		}


	// on stringify ce qu'il faut publier
		if(PP != pp_old || tempo > 300){

			//printf("brute PP: %d\n", PP);
			sprintf(str_pp, "%d", PP);
			rc = mosquitto_publish(mosq, NULL, "up/value/pp", strlen(str_pp), str_pp, 2, false);
			if(rc != MOSQ_ERR_SUCCESS){
				fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
			}

			pp_old = PP;

		}
	}
	// si branché et authentifié

	if(cp_activated && plugged){
		
		
		// pour commencer une charge
		if(CP == 0){
			mode_led = GREEN_BLINK;
			if(old_pwm != 100){
				printf("on mets le pwm a 100 (CP == 0)\n");
				pwmWrite(CP_PWM, 100);
				old_pwm = 100;
				usleep(150000);
			}
		}
		// avant une charge
		else if(CP == 12 && cp_old2 == 0){
			mode_led = GREEN_BLINK;

			if(old_pwm != 100){
				printf("on mets le pwm a 100 (CP == 12 && cp_old2 == 0)\n");
				pwmWrite(CP_PWM, 100);
				old_pwm = 100;
				usleep(150000);
			}

		}
		// prepare une charge
		else if(CP == 9 && (cp_old2 == 12 || cp_old2 == 0)){

			mode_led = GREEN_BLINK;
			// on ouvre les relais si jamais ils sont fermer on ne sait jamais
			if(type2_closed = 1){

				type2_closed = 0;
				expander_t* expander = expander_init(0x26); //Pour les relais
				expander_resetPinGPIO(expander,TYPE_2_NL1_ON);
				expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);

				printf("Les relais N, L2 et L3 de la prise type 2 sont ouvert\n");
				expander_closeAndFree(expander);
			}
			
			if(old_pwm != dutycycle){
				printf("on mets le pwm a %d (CP == 9 && (cp_old2 == 12 || cp_old2 == 0)\n", dutycycle);

				pwmWrite(CP_PWM, dutycycle);
				old_pwm = dutycycle;
				usleep(150000);
			}

		}
		else if((CP == 6 || CP == 3)){

			if(type2_closed = 0){
				mode_led = GREEN_CHENILLE;

				type2_closed = 1;
				expander_t* expander = expander_init(0x26); //Pour les relais
				expander_setPinGPIO(expander,TYPE_2_NL1_ON);
				expander_setPinGPIO(expander, TYPE_2_L2L3_ON);
				
				expander_resetPinGPIO(expander, LOCK_D);
				usleep(1);
				digitalWrite(LOCK_P,1);
				Sleep(1);
				digitalWrite(LOCK_P,0);
				

				printf("Les relais N, L2 et L3 de la prise type 2 ainsi que le lock sont fermes\n");
				expander_closeAndFree(expander);
			}

			if(mode_led == RAINBOW_CIRCLE){

				mode_led = RAINBOW_CIRCLE;
			}
			if(old_pwm != dutycycle){

				pwmWrite(CP_PWM, dutycycle);
				old_pwm = dutycycle;
				usleep(150000);
				printf("on mets le pwm a %d (CP == 6 || CP == 3)\n", dutycycle);

			}

		}
		else if (CP == -12){

			if(type2_closed = 1){
				mode_led = RED_CHENILLE;
				type2_closed = 0;
				expander_t* expander = expander_init(0x26); //Pour les relais
				expander_resetPinGPIO(expander,TYPE_2_NL1_ON);
				expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);
				
				expander_setPinGPIO(expander, LOCK_D);
				usleep(1);
				digitalWrite(LOCK_P,1);
				Sleep(1);
				digitalWrite(LOCK_P,0);
				

				printf("Les relais N, L2 et L3 de la prise type 2 ainsi que le lock sont ouvert\n");
				expander_closeAndFree(expander);
			}
			if(mode_led == RED_BLINK){

				mode_led = RED_BLINK;
			}
			if(old_pwm != 0){

				printf("on mets le pwm a 100 (CP = -12)\n");
				pwmWrite(CP_PWM, 0);
				old_pwm = 0;
				usleep(150000);
			}


		}
		else if (CP == 9 & (cp_old2 == 6 || cp_old2 == 3)){

			if(type2_closed = 1){

				type2_closed = 0;
				expander_t* expander = expander_init(0x26); //Pour les relais
				expander_resetPinGPIO(expander,TYPE_2_NL1_ON);
				expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);
				
				expander_setPinGPIO(expander, LOCK_D);
				usleep(1);
				digitalWrite(LOCK_P,1);
				Sleep(1);
				digitalWrite(LOCK_P,0);
				

				printf("Les relais N, L2 et L3 de la prise type 2 ainsi que le lock sont ouvert\n");
				expander_closeAndFree(expander);
			}
			// mode_led = ORANGE_BLINK;

		}
		
	}
	// si authentifié mais pas branché
	else if (cp_activated == 1 && plugged == 0){
		
		mode_led = GREEN_BLINK;


		// si debranchement en cours de charge en principe impossible grace au lock
		if(type2_closed = 1){
			mode_led = RED_CHENILLE;
			type2_closed = 0;
			expander_t* expander = expander_init(0x26); //Pour les relais
			expander_resetPinGPIO(expander,TYPE_2_NL1_ON);
			expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);

			printf("Les relais N, L2 et L3 de la prise type 2 sont ouvert\n");
			expander_closeAndFree(expander);
		}

		// on mets a 12v la ligne CP pour preparer au futur branchement
		if(old_pwm != 100){
			printf("on mets le pwm a 100 (cp_activated == 1 && plugged == 0)\n");
			pwmWrite(CP_PWM, 100);
			old_pwm = 100;
			usleep(150000);
		}

		if(CP == 12 && (cp_old2 == 6 || cp_old2 == 3)){
			if(type2_closed = 1){

				type2_closed = 0;
				expander_t* expander = expander_init(0x26); //Pour les relais
				expander_resetPinGPIO(expander,TYPE_2_NL1_ON);
				expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);

				printf("Les relais N, L2 et L3 de la prise type 2 sont ouvert\n");
				expander_closeAndFree(expander);
			}
			if(old_pwm != 100){
				printf("on mets le pwm a 100 (CP == 12 && (cp_old2 == 6 || cp_old2 == 3))\n");
				pwmWrite(CP_PWM, 100);
				old_pwm = 100;
				usleep(150000);
			}

		}
		

		

	}
	else if (cp_activated == 0 && plugged == 1){
		
		mode_led = BLUE_BLINK;
		
		// fin de charge
		if((CP == 6 && cp_old2 == 9) || (CP == 3 && cp_old2 == 9) || (CP == 3 && cp_old2 == 6) || (CP == 6 && cp_old2 == 3)){
			
			if(type2_closed = 1){

				type2_closed = 0;
				expander_t* expander = expander_init(0x26); //Pour les relais
				expander_resetPinGPIO(expander,TYPE_2_NL1_ON);
				expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);
				
				expander_setPinGPIO(expander, LOCK_D);
				usleep(1);
				digitalWrite(LOCK_P,1);
				Sleep(1);
				digitalWrite(LOCK_P,0);
				

				printf("Les relais N, L2 et L3 de la prise type 2 ainsi que le lock sont ouvert\n");
				expander_closeAndFree(expander);
			}
			
			
		}


		// après une charge
		else if (CP == 9 && (cp_old2 == 6 || cp_old2 == 3)){

			if(type2_closed = 1){

				type2_closed = 0;
				expander_t* expander = expander_init(0x26); //Pour les relais
				expander_resetPinGPIO(expander,TYPE_2_NL1_ON);
				expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);
				
				expander_setPinGPIO(expander, LOCK_D);
				usleep(1);
				digitalWrite(LOCK_P,1);
				Sleep(1);
				digitalWrite(LOCK_P,0);
				

				printf("Les relais N, L2 et L3 de la prise type 2 ainsi que le lock sont ouvert\n");
				expander_closeAndFree(expander);
			}

			// si on a deja mis le pwm a 100
			if(old_pwm == 100){
				printf("on mets le pwm a 0 car a 100(CP == 9 && (cp_old2 == 6 || cp_old2 == 3))\n");
				pwmWrite(CP_PWM, 0);
				old_pwm = 0;
				usleep(150000);
				
			}
			// si on a pas encore desenclencher une charge
			else if(old_pwm != 100){
				printf("on mets le pwm a 100 (CP == 9 && (cp_old2 == 6 || cp_old2 == 3))\n");
				pwmWrite(CP_PWM, 100);
				old_pwm = 100;
				usleep(150000);

			}
		}


		else if(CP != 0){
			printf("on mets le pwm a 0 car CP!=0 (cp_activated == 0 && plugged == 1)\n");
			pwmWrite(CP_PWM, 0);
			old_pwm = 0;
			usleep(150000);
			
		}
		

	}

	else if (cp_activated == 0 && plugged == 0){

		if(mode_led != 0 && mainled != 0x00FF00){
			mode_led = 0;
			mainled = 0x80000;

		}
		if(CP != 0 || old_pwm != 0){
			printf("on mets le pwm a 0 (cp_activated == 0 && plugged == 0)\n");
			pwmWrite(CP_PWM, 0);
			old_pwm = 0;
			usleep(150000);
			
		}
	}
	
	if(csv_activated){

		FILE* cpFile = fopen("/home/pi/cp.csv","a");
		FILE* ppFile = fopen("/home/pi/pp.csv","a");
		if(cpFile != NULL){
			if(cpt_csv < 1000){

				fprintf(cpFile, "%ld;%lf\n", time(NULL),cp );
			}
		}
		if(ppFile != NULL){
			if(cpt_csv < 1000){

				fprintf(ppFile, "%ld;%lf\n", time(NULL),pp );
				cpt_csv++;
			}
				
			else{
			
				csv_activated = 0;
			}
		}
		cpt_csv = 0;

		fclose(cpFile);
		fclose(ppFile);
	}

    //time_t seconds;

    //seconds = time(NULL);
	


	if(tempo > 300){



		sprintf(str_pp, "%d", pp_old);

		rc = mosquitto_publish(mosq, NULL, "up/value/pp", strlen(str_pp), str_pp, 2, false);
		if(rc != MOSQ_ERR_SUCCESS){
			fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
		}
		
		sprintf(str_cp, "%d", cp_old);

		rc = mosquitto_publish(mosq, NULL, "up/value/cp", strlen(str_cp), str_cp, 2, false);
		if(rc != MOSQ_ERR_SUCCESS){
			fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
		}

		temp = toDegres(readAdc(0,T_CS));
		sprintf(str_temp, "%lf", temp);
		rc = mosquitto_publish(mosq, NULL, "up/value/temp", strlen(str_temp), str_temp, 2, false);
		if(rc != MOSQ_ERR_SUCCESS){
			fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
		}
		

		if(s0_activated){
			
			char str_charge[128];
			char str_current[128];
			char str_power[128];
			sprintf(str_charge, "%d", eeprom_getWh());
			sprintf(str_current, "%.2lf", current);
			sprintf(str_power, "%.2lf", power);

			//printf("tic : %s\n", str_tic);
			
			rc = mosquitto_publish(mosq, NULL, "up/value/s0/charge", strlen(str_charge), str_charge, 2, false);
			if(rc != MOSQ_ERR_SUCCESS){
				fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
			}

			rc = mosquitto_publish(mosq, NULL, "up/value/s0/power", strlen(str_power), str_power, 2, false);
			if(rc != MOSQ_ERR_SUCCESS){
				fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
			}

			rc = mosquitto_publish(mosq, NULL, "up/value/s0/current", strlen(str_current), str_current, 2, false);
			if(rc != MOSQ_ERR_SUCCESS){
				fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
			}
			tempo = 0;
		}
	}

	
}

int main(int argc, char *argv[])
{


    if(wiringPiSetup() < 0)
	{
		fprintf(stderr, "fonction %s: Unable to set up WiringPi: %s\n", __func__, strerror(errno));
		exit(EXIT_FAILURE);
	}

// TODO : ecrire le code qui initialise les gpio de la rp et des expander etc
	pinMode(LOCK_P, OUTPUT);
	digitalWrite(LOCK_P, 0);


	pinMode(RCD_TRIP_DC, INPUT);
	pullUpDnControl(RCD_TRIP_DC, PUD_OFF);

	pinMode(RCD_TRIP_AC, INPUT);
	pullUpDnControl(RCD_TRIP_AC, PUD_OFF);

	
	pinMode(USER_KEY, INPUT);
	pullUpDnControl(USER_KEY, PUD_OFF);
	// peut etre le mettre dans un autre thread car pas fonctionnel pour le moment
	// wiringPiISR (USER_KEY, INT_EDGE_FALLING,  &user_key_pressed_interrupt) ;
	// wiringPiISR (USER_KEY, INT_EDGE_RISING,  &user_key_released_interrupt) ;
	wiringPiISR (USER_KEY, INT_EDGE_BOTH,  &user_key_interrupt) ;

	pinMode(SM_TIC_D, INPUT);
	pullUpDnControl(SM_TIC_D, PUD_OFF);
    wiringPiISR (SM_TIC_D, INT_EDGE_FALLING,  &S0_interrupt);
	


	pinMode(CF4, INPUT);
	pullUpDnControl(CF4, PUD_DOWN);
	pinMode(IRQ1, INPUT);
	pullUpDnControl(IRQ1, PUD_DOWN);
	pinMode(IRQ0, INPUT);
	pullUpDnControl(IRQ0, PUD_DOWN);



	// lock_fb
	pinMode(LOCK_FB, INPUT);
	pullUpDnControl(LOCK_FB, PUD_OFF);

	pinMode(WD_TRIP, OUTPUT);
	digitalWrite(WD_TRIP, 0);

	pinMode(PP_IN, INPUT);
	pullUpDnControl(PP_IN, PUD_OFF);

	pinMode (CP_PWM, PWM_OUTPUT) ; /* set PWM pin as output */
 	pwmSetClock (192);
 	pwmSetRange(100);
  	pwmSetMode(PWM_MODE_MS);
    pwmWrite(CP_PWM, 0);

	// softPwmCreate (SCREEN_PWM, 50, 100) ;

	pinMode(LED_STRIP_D, OUTPUT);

	// // on attend 10 secondes le temps que les services soient bien démarrés ( i2c par exemple ici)
	Sleep(10);
	expander_t* exp1 = expander_init(0x27);
	expander_t* exp2 = expander_init(0x26);
	expander_setPullup(exp1, 0XFF);
	expander_setPullup(exp1, 0XFF);
	expander_setAndResetSomePinsGPIO(exp1, 0b00111111);
	expander_setAndResetSomePinsGPIO(exp2, 0b11100000);
	expander_closeAndFree(exp1);
	expander_closeAndFree(exp2);
// on ouvre la prise on sait jamais
	digitalWrite(LOCK_P, 1);
	Sleep(1);
	digitalWrite(LOCK_P, 0);


	// declartion des variables
	
	int rc, tentatives = 0;
	struct timeval te; 
    time_t start, end;
	start = 0;
	pthread_t thread_obj;
	pthread_t thread_obj2;

// on configure l'execution de la fonction interruption si ctrl+C
	signal(SIGINT, nettoyage);


//création du thread du scan rfid
	pthread_create(&thread_obj, NULL, *thread_rfid, NULL);
	pthread_create(&thread_obj2, NULL, *thread_led, NULL);

// phase d'initialisation
	/* initialisation mosquitto, a faire avant toutes appels au fonction mosquitto */
	rc = mosquitto_lib_init();

	if(rc != MOSQ_ERR_SUCCESS){
		
		fprintf(stderr, "fonction %s: Error mosquitto_lib_init: %s\n",__func__,  mosquitto_strerror(rc));
		// close(fd);
		
		//return 1; on ne souhaite pas quitter la boucle
	}
	else{

			// on cree une instance mqtt en mode clean session
			mosq = mosquitto_new("hubload", true, NULL);

			// si pas cree on quitte pour cause d'erreur
			if(mosq == NULL){

				mosquitto_lib_cleanup();
				fprintf(stderr, "fonction %s: Error mosquitto_new: Out of memory\n",__func__);
				// close(fd);
				
				return 1;
			}
			else{

			// si tout va bien on configure les callback pour le debug 
			// a la connexion au broker on lance la fonction on_connect
				mosquitto_connect_callback_set(mosq, on_connect);
			// a la publication on lance la fonction on_publish
				mosquitto_publish_callback_set(mosq, on_publish);

                
            /* on configure la fonction a apelle sur un message recu */
                mosquitto_message_callback_set(mosq, on_message);

			// on se connecte au broker
				rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 5);

			// si probleme on quitte le programme avec un message d'erreur
				if(rc != MOSQ_ERR_SUCCESS){

					mosquitto_destroy(mosq);
					mosquitto_lib_cleanup();
					fprintf(stderr, "fonction %s: Error mosquitto_connect: %s\n", __func__, mosquitto_strerror(rc));
					// close(fd);

					
					return 1;
				
				}

			}
	}

	rc = mosquitto_publish(mosq, NULL, "up/wakeup", strlen("1"), "1", 2, false);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
	}
// fin de l'initialisation
	// debut de la boucle infini
	while(1){
		gettimeofday(&te, NULL); // get current time
		/* on garde la connexion active avec le broker */
		rc = mosquitto_loop(mosq,10,256);

		// si jamais erreur : exemple deconnexion du broker, reload du service mosquitto etc...
		// on relance la batterie d'initialisation après 30s d'attente et ce durant 5 tentatives
		if(rc != MOSQ_ERR_SUCCESS){
			
			// on detruit l'ancienne instance
			mosquitto_destroy(mosq);
			if(tentatives >= 5){

				printf("Arret du programme, impossible de fonctionner après 5 tentatives, verifier le service mosquitto\n");
				nettoyage(0);
				//return EXIT_FAILURE;
			}

			// affichage de l'erreur pour le debug
			fprintf(stderr, "fonction %s: Error mosquitto_loop: %s\n", __func__, mosquitto_strerror(rc));

			printf("%d tentatives, On attend durant 30s pour réessayer de se connecter au broker\n", ++tentatives);
			Sleep(30);

			// initialisation mosquitto
			rc = mosquitto_lib_init();
			if(rc != MOSQ_ERR_SUCCESS){
				
				fprintf(stderr, "fonction %s: Error mosquitto_lib_init: %s\n",__func__,  mosquitto_strerror(rc));
			}
			else{

				/* Creation d'une instance cliente mosquitto */
				mosq = mosquitto_new("adc", true, NULL);
				if(mosq == NULL){

					/* On affiche le message d'erreur*/
					fprintf(stderr, "fonction %s: Error mosquitto_new: Out of memory.\n",__func__ );
					/* On libere les fonctions utilisé*/
					mosquitto_lib_cleanup();

					/* On ferme le descripteur du SPI*/
					
				}
				else{

					/* Configure les callbacks */
					mosquitto_connect_callback_set(mosq, on_connect);
					mosquitto_publish_callback_set(mosq, on_publish);
                    mosquitto_message_callback_set(mosq, on_message);

					/* connexion au broker */
					rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 5);

					/* si erreur */
					if(rc != MOSQ_ERR_SUCCESS){

						mosquitto_destroy(mosq);
						
						mosquitto_lib_cleanup();

					/* On affiche le message d'erreur*/
						fprintf(stderr, "fonction %s: Error mosquitto_connect: %s\n", __func__, mosquitto_strerror(rc));
					}
					else{

						rc = mosquitto_publish(mosq, NULL, "up/wakeup", strlen("1"), "1", 2, false);
						if(rc != MOSQ_ERR_SUCCESS){
							fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
						}
					}

				}
			}


		}
			/* Si tout va bien on publie */
		else{

			usleep(1000);
            tentatives = 0;
			tempo++;
    		
           	publish_values(mosq);

		}	
		
	}

	nettoyage(0);
	return 0;
}