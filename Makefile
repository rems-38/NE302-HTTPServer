main : main.o verifMessage.o isX.o arbre.o
	gcc -Wall -Wextra -g -o $@ $^

arbre.o : arbre.c arbre.h
	gcc -Wall -Wextra -g -c $<

isX.o : isX.c isX.h arbre.h
	gcc -Wall -Wextra -g -c $<

verifMessage.o : verifMessage.c verifMessage.h isX.h arbre.h
	gcc -Wall -Wextra -g -c $<

main.o : main.c verifMessage.h isX.h arbre.h
	gcc -Wall -Wextra -g -c $<

clean : 
	rm -rf *.o main
