
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof((a)[0]))

/* Variables for struct spi_ioc_transfer */
typedef struct{
	uint8_t *tx;          // Tableau contenant les donnees a transmettre ( 8 octets)
	uint8_t rx[8];			// Tableau contenant les donnees a recuperer ( 8 octets)
	uint8_t mode;           // Mode de SPI           
	uint8_t bits;           // bits par transmition
	uint32_t speed;         // vitesse de communication en Hz ( 10 Mhz pour l'ADE )
	uint16_t delay;  		// Temps entre chaque paquets 

	char device[20];
	int fileDescriptor;
}spiData;

/******************************************************************************/
int spiInit(spiData *data);

/******************************************************************************/
int spiTransfer(spiData *data);
