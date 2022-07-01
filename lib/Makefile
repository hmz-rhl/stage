CC= gcc -Wall
AR= ar
NAME=
SRC= $(NAME).c
HDR= $(SRC:.c=.h)
OBJ= $(SRC:.c=.o)
LIB= lib$(SRC:.c=).a
DIR=

all: 	$(OBJ)
	sudo $(AR) -rv $(LIB) $(OBJ)

$(OBJ):	$(SRC)
	sudo $(CC) -c $(SRC) -o $(OBJ)

.PHONY: install
install:
	sudo cp $(LIB) $(DIR)/usr/lib;
	sudo cp $(HDR) $(DIR)/usr/include;

.PHONY: uninstall
uninstall:
	sudo rm -f $(DIR)/usr/lib/$(LIB);
	sudo rm -f $(DIR)/usr/include/$(HDR);

.PHONY: clean
clean:
	sudo rm -f *.o *.a