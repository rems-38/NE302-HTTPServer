# NB : On ne peut pas faire le `export LD_LIBRARY_PATH=./lib/` dans le Makefile

all:
	make -C parseur/
	make -C server/

clean:
	make -C parseur/ clean
	make -C server/ clean