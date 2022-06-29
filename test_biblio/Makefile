CC= gcc -Wall
AR= ar
SRC= foo.c
HDR= foo.h
OBJ= $(SRC:.c=.o)
LIB= lib$(SRC:.c=)

all: 	foo.o
	sudo $(AR) -rv $(LIB) $(OBJ)
foo.o:	foo.c
	sudo $(CC) -c $(SRC) -o $(OBJ)
install:
	sudo cp libfoo.a /usr/local/lib;
	sudo cp foo.h /usr/local/include;
uninstall:
	sudo rm -f /usr/local/lib/libfoo.a;
	sudo rm -f /usr/local/include/foo.h;
clean:
	sudo rm -f *.o *.a