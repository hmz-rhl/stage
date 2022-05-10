#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "pn532.h"
#include <time.h>
#include "PN532_Rpi_I2C.h"
#include "../lib/mqtt.h"


void traitement_scan(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg);

int main()
{
    struct mosquitto* mosq = init_mqtt();
    mqtt_subscribe("down/scan/activate",traitement_scan,mosq);
    mqtt_publish("up/scan","Un scan a été fait", mosq);
}


void traitement_scan(struct mosquitto *mosq, void* obj, const struct mosquitto_message* msg)
{
    uint8_t buff[255];
    uint8_t uid[MIFARE_UID_MAX_LENGTH];
    uint32_t pn532_error = PN532_ERROR_NONE;
    int32_t uid_len = 0;
    printf("Hello!\r\n");
    PN532 pn532;
    PN532_I2C_Init(&pn532);
    if (PN532_GetFirmwareVersion(&pn532, buff) == PN532_STATUS_OK) {
        printf("Found PN532 with firmware version: %d.%d\r\n", buff[1], buff[2]);
    } 
    else 
    {
        
    }
    PN532_SamConfiguration(&pn532);
    printf("Waiting for RFID/NFC card...\r\n");
    while (delay < 60) {
        delay = 
        // Check if a card is available to read
        uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 1000);
        if (uid_len == PN532_STATUS_ERROR) 
        {
            printf(".");
            fflush(stdout);
        } 
        else 
        {
            printf("Found card with UID: ");
            for (uint8_t i = 0; i < uid_len; i++) {
                printf("%02x ", uid[i]);
            }
            printf("\r\n");
            break;
        }
    }
}