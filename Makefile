main : TransferEncodingHeader.o arbre.o
	gcc -Wall -Wextra -g -o $@ $^

arbre.o : arbre.c arbre.h
	gcc -Wall -Wextra -g -c $<

TransferEncodingHeader.o : TransferEncodingHeader.c arbre.h
	gcc -Wall -Wextra -g -c $<

clean : 
	rm -rf *.o main

