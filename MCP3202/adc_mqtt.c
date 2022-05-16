/**
 * @file get-adc.c
 * @author Hamza RAHAL, Hicham GHANEM, Raphaël GANDUS, Thibault FLORES(you@domain.com)
 * @brief lecture et publication des valeurs des adc
 * @version 0.1
 * @date 2022-05-14
 * 
 * @copyright Saemload (c) 2022
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include <signal.h>
#include <mosquitto.h>

#include "../lib/MCP3202.h"



//variable globale

struct mosquitto* mosq;

// declaration et definitions des fonctions

// fonction qui passe de volt a degre pour le composant TMP36
double toDegres(int tension){

	if(tension < 0){

		printf("%s: ne peut pas convertir une tension negative en degres", __func__);
		return tension;
	}
	return (toMillivolt(tension)-500)/10;
}


// fonction a executer l'ors d'une interruption par ctrl+C
void interruption(int n)
{
	
	printf("%s: interruption on detruit l'instance mosq\n", __func__);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	exit(EXIT_SUCCESS);
}


/* Callback called when the client receives a CONNACK message from the broker. */
void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	/* Print out the connection result. mosquitto_connack_string() produces an
	 * appropriate string for MQTT v3.x clients, the equivalent for MQTT v5.0
	 * clients is mosquitto_reason_string().
	 */
	printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
	if(reason_code != 0){
		/* If the connection fails for any reason, we don't want to keep on
		 * retrying in this example, so disconnect. Without this, the client
		 * will attempt to reconnect. */
		mosquitto_disconnect(mosq);
	}

	/* You may wish to set a flag here to indicate to your application that the
	 * client is now connected. */
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


// fonction qui lit et publie les valeurs en mqtt
void publish_values(struct mosquitto *mosq)
{
	char payload[20];
	double pp, cp, PP, CP, TEMP;
	double temp, cp_reel,;
	int rc;
	char str_temp[100], str_cp[100],  str_pp[100];

// affiche sur la console

	printf("%s: Lecture de Temperature\n", __func__);
	//conversion en degres
	temp = toDegres(readAdc(0,T_CS));
	usleep(10);

// affiche sur la console

	printf("%s: Lecture de PP\n", __func__);
	//conversion en volt
	pp = toVolt(readAdc(0,PP_CS));
	usleep(10);
// affiche sur la console

	printf("%s: Lecture de CP\n", __func__);
	//conversion en volt
	cp = toVolt(readAdc(0,CP_CS));
	cp_reel = 4*cp;
	usleep(10);

	TEMP = (int)temp;

	


	sprintf(str_temp, "%d", TEMP);


	usleep(10);

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
	usleep(10);

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

// affichage sur la console
	printf("Ce qu'il doit être envoyé par MQTT:\n");
	printf("temp %d°C\n", TEMP);
	printf("cp %d\n", CP);
	printf("pp %d\n", PP);

// on stringify ce qu'il faut publier
	sprintf(str_pp, "%d", PP);
// affiche sur la console
	usleep(10);


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

}


int main(int argc, char *argv[])
{
	// declartion des variables
	
	int rc, tentatives = 0;

// on configure l'execution de la fonction interruption si ctrl+C
	signal(SIGINT, interruption);

	/* initialisation mosquitto, a faire avant toutes appels au fonction mosquitto */
	rc = mosquitto_lib_init();

	if(rc != MOSQ_ERR_SUCCESS){
		
		fprintf(stderr, "fonction %s: Error mosquitto_lib_init: %s\n",__func__,  mosquitto_strerror(rc));
		// close(fd);
		
		//return 1; on ne souhaite pas quitter la boucle
	}
	else{

			// on cree une instance mqtt en mode clean session
			mosq = mosquitto_new("adc", true, NULL);

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

	// debut de la boucle infini
	while(1){

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
			sleep(30);

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

					/* connexion au broker */
					rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 5);

					/* si erreur */
					if(rc != MOSQ_ERR_SUCCESS){

						mosquitto_destroy(mosq);
						
						mosquitto_lib_cleanup();

					/* On affiche le message d'erreur*/
						fprintf(stderr, "fonction %s: Error mosquitto_connect: %s\n", __func__, mosquitto_strerror(rc));
					}

				}
			}


		}
			/* Si tout va bien on publie */
		else{

			tentatives = 0;
			publish_values(mosq);
			/* on temporise */
			sleep(4);

		}	
		
	}

	
	return EXIT_SUCCESS;
}

 	


