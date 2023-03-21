CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined

myshell: myshell.o arraylist.o
	$(CC) $(CFLAGS) $^ -o $@

myshell.o: myshell.c myshell.h arraylist.o
	$(CC) $(CFLAGS) $^ -o $@

arraylist.o: arraylist.c arraylist.h
	$(CC) -c $(CFLAGS) -DSAFE -DDEBUG=2 $< -o $@
	
clean: 
	rm -rf myshell *.o
	