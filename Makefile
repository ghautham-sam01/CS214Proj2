CC = gcc

myshell: myshell.o
	gcc -o myshell myshell.o

myshell.o: myshell.c myshell.h
	gcc -c myshell.c

clean: 
	rm -f myshell *.o
	