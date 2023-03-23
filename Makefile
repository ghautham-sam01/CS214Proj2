CC = gcc
CFLAGS = -fsanitize=address

myshell: myshell.o
	$(CC) $(CFLAGS) myshell.o -o myshell

myshell.o: myshell.c
	$(CC) $(CFLAGS) -c myshell.c

clean: 
	rm -rf myshell *.o