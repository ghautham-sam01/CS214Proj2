CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address 

myshell: words.o readline.o arraylist.o commands.o myshell.o 
	$(CC) $(CFLAGS) $^ -o $@

myshell.o: myshell.c myshell.h
	$(CC) $(CFLAGS) -c myshell.c

commands.o: commands.c commands.h
	$(CC) $(CFLAGS) -c commands.c

arraylist.o: arraylist.c arraylist.h
	$(CC) $(CFLAGS) -c arraylist.c

readline.o: readline.c readline.h
	$(CC) $(CFLAGS) -c readline.c

words.o: words.c words.h
	$(CC) $(CFLAGS) -c words.c

clean: 
	rm -rf  *.o myshell
	