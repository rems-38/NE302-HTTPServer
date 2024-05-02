
all : simple

%.o : %.c
	gcc -g -fsanitize=address  -I../librequest/api -I libparser -c $< 

simple: simple.o ../librequest-0.5/librequest.so 
	gcc -g -fsanitize=address  -o simple simple.o -L. -lrequest -lparser 

etape3.tar.gz : simple simple.c Makefile
	tar cvzf etape-3.tar.gz $^

clean : 
	rm -rf sock
	rm -rf *.o 

