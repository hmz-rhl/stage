/**
 * @file hubload.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.2
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
#include <wiringPi.h> // compilation ajouter -lwiringPi
#include <pthread.h> // compilation ajouter -lptrhread
#include <softPwm.h>
#include "../pn532/pn532.h"
#include "../pn532/PN532_Rpi_I2C.h"


#include "../lib/MCP3202.h"

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


struct mosquitto *mosq;
int dutycycle;
uint8_t scan_activated = 0;
int user_key_clicked = 0;
unsigned long long compteur_tic = 0;
unsigned long long historique_Wh = 0;

void Sleep(uint32_t time) {

	usleep(1000000*time);
}

void user_key_interrupt(void){

	printf("%s: Début de routine\n",__func__);
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
	printf("%s: Fin de routine\n",__func__);
}

void tic_interrupt(void){

	printf("%s: debut de routine\n",__func__);
	compteur_tic++;
	printf("%s: Fin de routine\n",__func__);


}

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


        

	while(1){

		if(scan_activated){
			
			Sleep(1);

			while(PN532_I2C_Init(&pn532) < 0){

				Sleep(1);
			}
   			while(PN532_GetFirmwareVersion(&pn532, buff) != PN532_STATUS_OK) {
		
				Sleep(1);
    		}

			printf("Found PN532 with firmware version: %d.%d\r\n", buff[1], buff[2]);
    		PN532_SamConfiguration(&pn532);

			printf("Waiting for RFID/NFC card...\r\n");
			while(1)
			{
			
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
					printf("%02x . %02x . %02x . %02x . %02x . %02x . %02x . %02x \n",uid[0],uid[1],uid[2],uid[3],uid[4],uid[5],uid[6],uid[7]);
					
					sprintf(message, "%02x . %02x . %02x . %02x . %02x . %02x . %02x . %02x", uid[0],uid[1],uid[2],uid[3],uid[4],uid[5],uid[6],uid[7]);
					mosquitto_publish(mosq,NULL,"up/scan",strlen(message),message,2,false);
					printf("\r\n");
					
					//scan_activated = 0;
					break;
								
				}


			}
		
		}
		else{
			Sleep(1);
		}


	}
}

// fonction a executer lors d'une interruption par ctrl+C
void nettoyage(int n)
{
	
	printf("%s: interruption on detruit l'instance mosq\n", __func__);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	exit(EXIT_SUCCESS);
}

// fonction qui passe de volt a degre pour le composant TMP36
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
    char *topics[10]= {"down/type_ef/open","down/type_ef/close","down/type2/open","down/type2/close","down/charger/pwm","down/lockType2/open","down/lockType2/close","down/scan/activate","down/scan/shutdown","down/tic/reset"};

    rc = mosquitto_subscribe_multiple(mosq,NULL,10,topics,2,0,NULL);
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
	}
}



/* Callback called when the client knows to the best of its abilities that a
 * PUBLISH has been successfully sent. For QoS 0 this means the message has
 * been completely written to the operating system. For QoS 1 this means we
 * have received a PUBACK from the broker. For QoS 2 this means we have
 * received a PUBCOMP from the broker. */
void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	printf("%s: Message %d has been published.\n\n",__func__, mid);
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
    }

    else if(!strcmp(msg->topic,"down/type_ef/close")){
        expander_t* expander = expander_init(0x26); //Pour les relais
        expander_setPinGPIO(expander, TYPE_E_F_ON);
        printf("Le relais de la prise E/F est ferme\n");
        expander_closeAndFree(expander);
    }

    else if(!strcmp(msg->topic,"down/type2/close")){

        if(!strcmp(msg->payload, "1")){
            expander_t* expander = expander_init(0x26); //Pour les relais
            expander_setPinGPIO(expander,TYPE_2_NL1_ON);
            expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);

            printf("Les relais N et L1 de la prise type 2 sont fermes\n");
            expander_closeAndFree(expander);
        }
        else if(!strcmp(msg->payload, "3")){
            expander_t* expander = expander_init(0x26); //Pour les relais
            expander_setPinGPIO(expander,TYPE_2_NL1_ON);
            expander_setPinGPIO(expander, TYPE_2_L2L3_ON);

            printf("Les relais N, L2 et L3 de la prise type 2 sont fermes\n");
            expander_closeAndFree(expander);

        }

    }

    else if(!strcmp(msg->topic,"down/type2/open")){

        if(!strcmp(msg->payload, "1")){
            expander_t* expander = expander_init(0x26); //Pour les relais
            expander_resetPinGPIO(expander, TYPE_2_NL1_ON);
            // expander_resetPinGPIO(expander, 1);
            printf("Les relais N et L1 de la prise type 2 sont ouverts\n");
            expander_closeAndFree(expander);
        }
        else if(!strcmp(msg->payload, "3")){
            expander_t* expander = expander_init(0x26); //Pour les relais
            expander_resetPinGPIO(expander, TYPE_2_NL1_ON);
            expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);
            printf("Les relais L2 et L3 de la prise type 2 sont ouvert\n");
            expander_closeAndFree(expander);
        }
    
    }

    else if(!strcmp(msg->topic, "down/charger/pwm")){

        
        dutycycle = atoi(msg->payload);

        if(dutycycle >= 0 && dutycycle <= 100){
            
            printf("dutycycle %d \n",dutycycle);
        }
		softPwmWrite(CP_PWM, dutycycle/10);

    }
    
    else if(!strcmp(msg->topic,"down/lockType2/open")){

		expander_t* expander = expander_init(0x26); //Pour les relais
		expander_setPinGPIO(expander, LOCK_D);

		digitalWrite(LOCK_P,1);

		sleep(1);
		digitalWrite(LOCK_P,0);

		expander_closeAndFree(expander);
        printf("Le moteur est ouvert\n");
    }
    else if(!strcmp(msg->topic,"down/lockType2/close")){
		expander_t* expander = expander_init(0x26); //Pour les relais
		expander_resetPinGPIO(expander, LOCK_D);
		digitalWrite(LOCK_P,1);

		Sleep(1);
		digitalWrite(LOCK_P,0);

		expander_closeAndFree(expander);
		
        printf("Le moteur est ferme\n");
    }

	else if( !strcmp(msg->topic, "down/scan/activate"))
	{
		scan_activated = 1;
	}

	else if( !strcmp(msg->topic, "down/scan/shutdown"))
	{
		scan_activated = 0;
	}

	else if(!strcmp(msg->topic, "down/tic/reset")){
		
		compteur_tic = 0;
	}



}

// fonction qui lit et publie les valeurs en mqtt
void publish_values(struct mosquitto *mosq)
{
	char payload[20];
	double pp, cp, temp, cp_reel;
	int PP, CP;

	// variable pour debug mqtt
	int rc;
	char str_temp[100], str_cp[100],  str_pp[100];

// affiche sur la console

	printf("%s: Lecture de Temperature\n", __func__);
	//conversion en degres
	temp = toDegres(readAdc(0,T_CS));
	usleep(50000);

// affiche sur la console

	printf("%s: Lecture de CP\n", __func__);
	//conversion en volt
	cp = toVolt(readAdc(0,CP_CS));
	cp_reel = 4.0*cp;
	usleep(50000);

// affiche sur la console

	printf("%s: Lecture de PP\n", __func__);
	//conversion en volt
	pp = toVolt(readAdc(0,PP_CS));
	usleep(50000);

	sprintf(str_temp, "%lf", temp);

	
	printf("___les tensions et temperature reeles recues chez l'adc___\n\n", cp_reel);

	printf("temperature: %lf°C\n", temp);
	printf("pp: %lfV\n", pp);
	printf("cp_reel: %lfV\n\n", cp_reel);

// on donne a CP les vraies valeurs correspondantes 
	CP = -12;
	if (cp_reel > 9.5){

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

// on stringify ce qu'il faut publier
	sprintf(str_cp, "%d", CP);
// affiche sur la console

// on donne a PP les valeurs correspondantes 
	if (pp < 0.58){

		PP = 80;
	}
	else if( pp < 0.9 ){

		PP = 63;
	}
	else if( pp < 1.5 ){

		PP = 32;
	}
	else if( pp < 2.2 ){

		PP = 20;
	}
	else if( pp < 2.6 ){

		PP = 13;
	}
	else{
		
		PP = 6;
	}


// on stringify ce qu'il faut publier
	sprintf(str_pp, "%d", PP);
// affiche sur la console

// affichage sur la console
	printf("___l'interpretation qui doit être envoyé par MQTT___\n");
	printf("temp %s°C\n", str_temp);
	printf("cp %s\n", str_cp);
	printf("pp %s\n", str_pp);

	/* Publish the message
	 * mosq - our client instance
	 * *mid = NULL - we don't want to know what the message id for this message is
	 * topic = "example/temperature" - the topic on which this message will be published
	 * payloadlen = strlen(payload) - the length of our payload in bytes
	 * payload - the actual payload
	 * qos = 2 - publish with QoS 2 for this example
	 * retain = false - do not use the retained message feature for this message
	 */
	rc = mosquitto_publish(mosq, NULL, "up/value/temp", strlen(str_temp), str_temp, 2, false);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
	}
	
	rc = mosquitto_publish(mosq, NULL, "up/value/pp", strlen(str_pp), str_pp, 2, false);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
	}

	rc = mosquitto_publish(mosq, NULL, "up/value/cp", strlen(str_cp), str_cp, 2, false);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
	}

	char str_tic[128];
	sprintf(str_tic, "%llu", compteur_tic);
	//printf("tic : %s\n", str_tic);
	rc = mosquitto_publish(mosq, NULL, "up/value/tic", strlen(str_tic), str_tic, 2, false);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "fonction %s: Error mosquitto_publish: %s\n", __func__, mosquitto_strerror(rc));
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
    wiringPiISR (SM_TIC_D, INT_EDGE_FALLING,  &tic_interrupt);


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

	pinMode(CP_PWM,PWM_OUTPUT);

	pinMode(LED_STRIP_D, OUTPUT);

	// // on attend 10 secondes le temps que les services soient bien démarrés ( i2c par exemple ici)
	Sleep(10);
	expander_t* exp1 = expander_init(0x27);
	expander_t* exp2 = expander_init(0x26);
	expander_setPullup(exp1, 0b00000000);
	expander_setPullup(exp1, 0b00000000);
	expander_setAndResetSomePinsGPIO(exp1, 0b11111111);
	expander_setAndResetSomePinsGPIO(exp2, 0b11111000);
	expander_closeAndFree(exp1);
	expander_closeAndFree(exp2);
// on ouvre la prise on sait jamais
	digitalWrite(LOCK_P, 1);
	Sleep(1);
	digitalWrite(LOCK_P, 0);
	

	if(softPwmCreate (CP_PWM,10 ,10)<0){

		fprintf(stderr, "fonction %s: Unable to set up PWM: %s\n", __func__, strerror(errno));
		exit(EXIT_FAILURE);

	}


	// declartion des variables
	
	int rc, tentatives = 0;
	struct timeval te; 
    time_t start, end;
	double delay = 0;
	start = 0;
	pthread_t thread_obj;

// on configure l'execution de la fonction interruption si ctrl+C
	signal(SIGINT, nettoyage);


//création du thread du scan rfid
	pthread_create(&thread_obj, NULL, *thread_rfid, NULL);

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
				fprintf(stderr, "fonction %s: Error mosquitto_new: Out of memo__func__, ry.\n");
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
				// close(fd);
				return EXIT_FAILURE;
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
					fprintf(stderr, "fonction %s: Error mosquitto_new: Out of memo__func__, ry.\n");
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

			Sleep(1);
			
            tentatives = 0;

    		
            end = te.tv_sec;
		    delay = end-start;

            if(delay > 4){

                start = end;
			    publish_values(mosq);

            }

		}	
		
	}

	nettoyage(0);
	return 0;
}