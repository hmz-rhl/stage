CC= gcc -Wall
AR= ar
NAME= maLibrairie
SRC= $(NAME).c
HDR= $(SRC:.c=.h)
OBJ= $(SRC:.c=.o)
LIB= lib$(SRC:.c=).a

all: 	$(OBJ)
	sudo $(AR) -rv $(LIB) $(OBJ)
$(OBJ):	$(SRC)
	sudo $(CC) -c $(SRC) -o $(OBJ)
install:
	sudo cp $(LIB) /usr/local/lib;
	sudo cp $(HDR) /usr/local/include;
uninstall:
	sudo rm -f /usr/local/lib/$(LIB);
	sudo rm -f /usr/local/include/$(HDR);
clean:
	sudo rm -f *.o *.a