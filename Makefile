CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address 

myshell: words.o readline.o arraylist.o myshell.o
	$(CC) $(CFLAGS) $^ -o $@

myshell.o: myshell.c myshell.h
	$(CC) $(CFLAGS) -c myshell.c

arraylist.o: arraylist.c arraylist.h
	$(CC) $(CFLAGS) -c arraylist.c

readline.o: readline.c readline.h
	$(CC) -c readline.c

words.o: words.c words.h
	$(CC) -c words.c

clean: 
	rm -rf  *.o myshell
	