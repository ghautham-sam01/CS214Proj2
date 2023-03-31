CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address 

myshell: words.o readline.o arraylist.o commands.o myshell.o 
	$(CC) $(CCFLAGS) $^ -o $@

test: words.o readline.o arraylist.o commands.o test.o
	$(CC) $(CCFLAGS) $^ -o $@

test.o: test.c
	$(CC) $(CCFLAGS) -c test.c

myshell.o: myshell.c myshell.h
	$(CC) $(CCFLAGS) -c myshell.c

commands.o: commands.c commands.h
	$(CC) $(CCFLAGS) -c commands.c

arraylist.o: arraylist.c arraylist.h
	$(CC) $(CCFLAGS) -c arraylist.c

readline.o: readline.c readline.h
	$(CC) $(CCFLAGS) -c readline.c

words.o: words.c words.h
	$(CC) $(CCFLAGS) -c words.c

clean: 
	rm -rf  *.o myshell
	