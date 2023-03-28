CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address 

myshell: arraylist.o myshell.o
	$(CC) $(CFLAGS) $^ -o $@

myshell.o: myshell.c myshell.h
	$(CC) $(CFLAGS) -c myshell.c

arraylist.o: arraylist.c arraylist.h
	$(CC) $(CFLAGS) -c arraylist.c

clean: 
	rm -rf  *.o myshell
	