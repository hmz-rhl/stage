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
	sudo gcc expander-get-gpio.o -o expander-get-gpio -Wall -lHubload;
	sudo gcc expander-set-gpio.o -o expander-set-gpio -Wall -lHubload -lm;
	make clean;
	

expander-get-gpio.o:	expander-get-gpio.c
	@echo "generation de l'objet...";
	sudo $(CC) -o $@ -c $<;

expander-set-gpio.o:	expander-set-gpio.c
	@echo "generation de l'objet...";
	sudo $(CC) -o $@ -c $<;

.PHONY: install
install:
	@echo "installation des commandes";
	sudo cp expander-get-gpio expander-set-gpio /usr/bin;
	@echo "checking des installations";
# cd ../test; 
	expander-get-gpio 26;
	expander-set-gpio -h;
# cd ../;

.PHONY: uninstall
uninstall:
	sudo rm -f /usr/lib/expander_i2c.a /usr/lib/MCP3202.a /usr/lib/rtc_eeprom.a;
	sudo rm -f /usr/include/expander_i2c.h /usr/include/MCP3202.h /usr/include/rtc_eeprom.h;

.PHONY: clean
clean:
	sudo rm -f *.o *.a