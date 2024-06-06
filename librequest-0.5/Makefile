FLAGS = -Wall -Wextra -fPIC -fsanitize=address
LINK =

DEBUG ?= yes
VALGRIND ?= no

ifeq ($(DEBUG), yes)
	FLAGS += -g
endif

ifeq ($(VALGRIND), yes)
	FLAGS = -Wall -Wextra
	LINK += -lasan -lubsan
endif

all : librequest.so sock

sock : librequest.so main.o reponse.o php.o
	gcc $(FLAGS) -o ../sock main.o reponse.o php.o -L. -lrequest -lparser

%.o: %.c 
	gcc $(FLAGS) $(LINK) -I../../abnf -I./api -I libparser -c $<

#librequest.so : socket.o 
#	ld -shared -Bsymbolic -Bsymbolic-function -soname librequest.so -o librequest.so ./parser/librequest.o ./socket.o 
#	gcc -Wall -shared -I./abnf -I ./api -o librequest.so socket.o ./parser/librequest.o 

clean : 
	rm -rf rfc*.c 
	rm -rf rfc*.h 
	rm -rf httpparser
	rm -rf *.o 
#	rm -rf *.so 

