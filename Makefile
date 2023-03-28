CC = gcc
CFLAGS = -fsanitize=address

myshell: myshell.o words.o
	$(CC) myshell.o words.o -o myshell

word: usewords.o words.o
	gcc words.o usewords.o -o word

tester: revline.o words.o
	gcc revline.o words.o -o tester

usewords.o: usewords.c
	gcc -c usewords.c

words.o: words.c
	gcc -c words.c

revline.o: revline.c
	gcc -c revline.c

myshell.o: myshell.c
	$(CC) -c myshell.c

clean: 
	rm -rf myshell *.o