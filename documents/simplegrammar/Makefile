CC = gcc
FLAGS = -Wall -Wextra -g

all: sg test


sg: sg.o
	$(CC) $(FLAGS) $^ -o $@

sg.o: sg.c
	$(CC) $(FLAGS) -c $^


test: test.o
	$(CC) $(FLAGS) $^ -o $@

test.o: test.c
	$(CC) $(FLAGS) -c $^


.PHONY: clean
clean: 
	rm sg sg.o test test.o