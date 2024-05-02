
all : librequest.so sock 

sock : librequest.so main.o reponse.o
	gcc -g -fsanitize=address -o sock main.o reponse.o -L. -lrequest -lparser

%.o: %.c 
	gcc -fPIC -fsanitize=address -g -I../../abnf -I./api -I libparser -c $<

#librequest.so : socket.o 
#	ld -shared -Bsymbolic -Bsymbolic-function -soname librequest.so -o librequest.so ./parser/librequest.o ./socket.o 
#	gcc -Wall -shared -I./abnf -I ./api -o librequest.so socket.o ./parser/librequest.o 

clean : 
	rm -rf rfc*.c 
	rm -rf rfc*.h 
	rm -rf httpparser
	rm -rf *.o 
#	rm -rf *.so 

