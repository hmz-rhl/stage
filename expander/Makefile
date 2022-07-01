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
#	make clean;
	

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
	sudo rm -f /usr/bin/expander-get-gpio /usr/bin/expander-set-gpio;

.PHONY: clean
clean:
	sudo rm -f *.o *.a