#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPi.h>

#include <time.h>
#define CP_PWM 23

int dutycycle,t,Ltime,Htime;

void wait_microSec(int delay)
{
	for(int i = 0; i< 1000*delay; i++) asm("nop;");
}
/* Callback called when the client receives a CONNACK message from the broker. */
void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	int rc;
	/* Print out the connection result. mosquitto_connack_string() produces an
	 * appropriate string for MQTT v3.x clients, the equivalent for MQTT v5.0
	 * clients is mosquitto_reason_string().
	 */
	printf("%s: %s\n", __func__,mosquitto_connack_string(reason_code));
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
    char topic[]= "down/charger/pwm";

    rc = mosquitto_subscribe(mosq,NULL,topic,2);
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

    printf("Nouveau message du topic %s: %s\n", msg->topic, (char *) msg->payload);



    dutycycle = atoi(msg->payload);
	printf("dutycycle %d \n",dutycycle);
	Htime = dutycycle*1000000000/100;
	Ltime = (100-dutycycle)*1000000000/100;
	// pwmWrite(CP_PWM, dutycycle*1023/100);
	

}


int main(int argc, char *argv[])
{
	struct mosquitto *mosq;
	int rc;
    dutycycle = 100;

	if(wiringPiSetup() < 0)
	{
		fprintf(stderr, "fonction %s: Unable to set up: %s\n", __func__, strerror(errno));
	}

    pinMode(CP_PWM, OUTPUT);

	//softPwmCreate (CP_PWM, 0, 100);

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
	//mosquitto_loop_forever(mosq, -1, 1);
	int res,val = 1;
	struct timespec now;
	long ns_prec;
	res = clock_gettime(CLOCK_REALTIME, &now);
	if(res != 0)
	{
		fprintf(stderr, "Error: %s\n", errno);
	}
	ns_prec = now.tv_nsec;
	t = 1000000;

    while(1){

        mosquitto_loop(mosq,10,256);

        digitalWrite(CP_PWM,val);
        
		
    	res = clock_gettime(CLOCK_REALTIME, &now);
		if(res != 0)
		{
			fprintf(stderr, "Error: %s\n", errno);
		}
		if(now.tv_nsec - ns_prec > t )
		{
			
			val = !val;
			ns_prec = now.tv_nsec;
			t = t==Htime ? Ltime:Htime;
		} 
			

        
    }

	mosquitto_lib_cleanup();
	return 0;
}