#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <wait.h>

#include "arraylist.h"
#include "myshell.h"
#include "readline.h"

#ifndef DEBUG
#define DEBUG 0
#endif
#ifndef BUFSIZE
#define BUFSIZE 512
#endif

/**
int interactiveMode(){
	char *line;
	int fin = STDIN_FILENO;
	do {
		write(1, "mysh> ", 7);
	} while((line = readLine(fin)));

	return EXIT_SUCCESS;
}

int batchMode(char *fileName){
	char *line;
	int fin = open(fileName, O_RDONLY);
	while((line = readLine(fin))) {

	}
	return EXIT_SUCCESS;
}
*/

int main(int argc, char **argv){
    printf("Hello! Welcome to our shell\n");
    if(argc==1){
    //Interactive mode
        printf("\nType some commands to begin\n");        
        interactiveMode();
    }else if(argc==2){
    //Batch mode
        printf("\nProcessing file....\n");
        batchMode(argv[1]);
    }else{
        printf("\n Too many args");
    }
	
    return 0;
}