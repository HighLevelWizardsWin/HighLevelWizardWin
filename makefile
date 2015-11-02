CC=gcc
CFLAGS=-I.
#DEPS = hellomake.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

project1: cube.o wizard.o
	#gcc -o theCube cube.o wizard.o -I.
	gcc -o theCube cube.o wizard.o -lreadline -lhistory -lncurses -lpthread -I.
	
run: cube.o wizard.o
	./theCube -size 2 -seed 2 -teamA 2 -teamB 2
	
clean:
	rm cube.o 
	rm wizard.o
	rm theCube