CC= gcc -Wall
AR= ar -rv
NAME= Hubload
SRC= $(NAME).c
HDR= $(SRC:.c=.h)
OBJ= $(SRC:.c=.o)
LIB= lib$(SRC:.c=).a
DIR=

all:	expander-get-gpio.o expander-set-gpio.o
#	echo "On change de repertoire -> /lib";
#	cd ./lib;
	sudo gcc expander-get-gpio
	

expander_i2c.o:	expander_i2c.c
	echo "generation de l'objet...";
	sudo $(CC) -o $@ -c $<;

.PHONY: install
install:
	echo "installation des bibliothèques";
	sudo cp *.a $(DIR)/usr/lib;
	sudo cp *.h $(DIR)/usr/include;
	echo "l'installation fini";
	echo "checking des installations";
# cd ../test; 
	sudo gcc ../test/test.c -o ../test/test.exe -Wall -lHubload;
	sudo ../test/test.exe;
# cd ../;

.PHONY: uninstall
uninstall:
	sudo rm -f $(DIR)/usr/lib/expander_i2c.a $(DIR)/usr/lib/MCP3202.a $(DIR)/usr/lib/rtc_eeprom.a;
	sudo rm -f $(DIR)/usr/include/expander_i2c.h $(DIR)/usr/include/MCP3202.h $(DIR)/usr/include/rtc_eeprom.h;

.PHONY: clean
clean:
	sudo rm -f *.o *.a