CC= gcc -Wall
AR= ar
NAME= maLibrairie
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
	sudo cp $(LIB) $(DIR)/usr/local/lib;
	sudo cp $(HDR) $(DIR)/usr/local/include;
.PHONY: uninstall
uninstall:
	sudo rm -f $(DIR)/usr/local/lib/$(LIB);
	sudo rm -f $(DIR)/usr/local/include/$(HDR);
.PHONY: clean
clean:
	sudo rm -f *.o *.a