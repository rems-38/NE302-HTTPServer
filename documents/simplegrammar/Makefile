CC = gcc
FLAGS = -Wall -Wextra -g


sg: sg.o
	$(CC) $(FLAGS) $^ -o $@

sg.o: sg.c
	$(CC) $(FLAGS) -c $^


.PHONY: clean
clean: 
	rm sg sg.o