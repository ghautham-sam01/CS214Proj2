CC = gcc

myshell: myshell.o
	gcc -o myshell myshell.o

myshell.o: myshell.c myshell.h
	gcc -c myshell.CC

clean: 
	rm -f myshell *.o
	