CC = gcc
FLAGS = -Wall -Wextra -g

main : main.o isX.o arbre.o
	$(CC) $(FLAGS) $^ -o $@
	

main.o : main.c isX.h arbre.h
	$(CC) $(FLAGS) -c $^

isX.o : isX.c isX.h arbre.h
	$(CC) $(FLAGS) -c $^

arbre.o : arbre.c arbre.h
	$(CC) $(FLAGS) -c $^


clean : 
	rm -rf *.o *.gch main
