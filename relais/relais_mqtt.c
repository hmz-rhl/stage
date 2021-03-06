#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "../lib/expander-i2c.h"


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
    char *topics[4]= {"down/type_ef/open","down/type_ef/close","down/type2/open","down/type2/close"};

    rc = mosquitto_subscribe_multiple(mosq,NULL,4,topics,2,0,NULL);
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


/* Callback called when the client receives a message. */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	/* This blindly prints the payload, but the payload can be anything so take care. */
	printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
    expander_t* expander = expander_init(0x26); //Pour les relais

    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);

    if(!strcmp(msg->topic,"down/type_ef/open")){

        expander_resetPinGPIO(expander, TYPE_E_F_ON); 
        printf("Le relais de la prise E/F est ouvert\n");
    }
    else if(!strcmp(msg->topic,"down/type_ef/close")){

        expander_setPinGPIO(expander, TYPE_E_F_ON);
        printf("Le relais de la prise E/F est ferme\n");
    }

    else if(!strcmp(msg->topic,"down/type2/close")){

        if(!strcmp(msg->payload, "1")){

            expander_setPinGPIO(expander,TYPE_2_NL1_ON);
            expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);

            printf("Les relais N et L1 de la prise type 2 sont fermes\n");
        }
        else if(!strcmp(msg->payload, "3")){
            
            expander_setPinGPIO(expander,TYPE_2_NL1_ON);
            expander_setPinGPIO(expander, TYPE_2_L2L3_ON);

            printf("Les relais N, L2 et L3 de la prise type 2 sont fermes\n");

        }

    }

    else if(!strcmp(msg->topic,"down/type2/open")){

        if(!strcmp(msg->payload, "1")){
            expander_resetPinGPIO(expander, TYPE_2_NL1_ON);
            // expander_resetPinGPIO(expander, 1);
            printf("Les relais N et L1 de la prise type 2 sont ouverts\n");
        }
        else if(!strcmp(msg->payload, "3")){
            expander_resetPinGPIO(expander, TYPE_2_NL1_ON);
            expander_resetPinGPIO(expander, TYPE_2_L2L3_ON);
            printf("Les relais L2 et L3 de la prise type 2 sont ouvert\n");

        }
    
    }
    
    expander_closeAndFree(expander);
}


int main(int argc, char *argv[])
{
	struct mosquitto *mosq;
	int rc;

	/* Required before calling other mosquitto functions */
	mosquitto_lib_init();

	/* Create a new client instance.
	 * id = NULL -> ask the broker to generate a client id for us
	 * clean session = true -> the broker should remove old sessions when we connect
	 * obj = NULL -> we aren't passing any of our private data for callbacks
	 */
	mosq = mosquitto_new(NULL, true, NULL);
	if(mosq == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}

	/* Configure callbacks. This should be done before connecting ideally. */
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_subscribe_callback_set(mosq, on_subscribe);
	mosquitto_message_callback_set(mosq, on_message);

	/* Connect to test.mosquitto.org on port 1883, with a keepalive of 60 seconds.
	 * This call makes the socket connection only, it does not complete the MQTT
	 * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
	 * mosquitto_loop_forever() for processing net traffic. */
	rc = mosquitto_connect(mosq, "localhost", 1883, 60);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

	/* Run the network loop in a blocking call. The only thing we do in this
	 * example is to print incoming messages, so a blocking call here is fine.
	 *
	 * This call will continue forever, carrying automatic reconnections if
	 * necessary, until the user calls mosquitto_disconnect().
	 */
	mosquitto_loop_forever(mosq, -1, 1);

	mosquitto_lib_cleanup();
	return 0;
}