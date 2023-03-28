#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include "myshell.h"
#include "arraylist.h"
#include "words.h"
#define BUFSIZE 512
#define DEBUG 1

// tokenizer layer
// parse layer
// execute layer

int main(int argc, char **argv)
{
	printf("Hello! Welcome to our shell\n");
	if (argc == 1)
	{
		// Interactive mode
		printf("\nType some commands to begin\n");
		interactiveMode();
	}
	else if (argc == 2)
	{
		// Batch mode
		printf("batch mode\n");
		batchMode(argv[1]);
	}
	else
	{
		printf("\n Too many args");
	}
	return 0;
}

int interactiveMode()
{
	char *line;
	char **args;
	while (line != 'q')
	{
		printf("myshell> ");
		// line = readLine();
		// args = getTokens(line);
		// execShell(args);
		// free(line);
		// free(args);
	}
	return 1;
}

int batchMode(char *fileName)
{
	// should go line by line in the file
	// tokenize each line
	// execute the appropriate program with the arguments
	

	return EXIT_SUCCESS;
}