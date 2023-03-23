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

//tokenizer layer
//parse layer
//execute layer

int main(int argc, char **argv){
    printf("Hello! Welcome to our shell\n");
    if(argc==1){
    //Interactive mode
        printf("\nType some commands to begin\n");        
        interactiveMode();
    }else if(argc==2){
    //Batch mode
		printf("batch mode\n");
        batchMode(argv[1]);
    }else{
        printf("\n Too many args");
    }
    return 0;
}

int interactiveMode(){
    char *line;
    char **args;
    while(line !='q'){
        printf("myshell> ");
        //line = readLine();
        //args = getTokens(line);
        //execShell(args);
        //free(line);
        //free(args);
    }
	return 1;
}

int batchMode(char *fileName){
    //should go line by line in the file
    //tokenize each line
    //execute the appropriate program with the arguments
    char *lineBuffer;
    int linePos, lineSize, bytes, pos;
	pos = 0;
    char buffer[BUFSIZE];
    int fd = open(fileName, O_RDONLY);
    lineBuffer = malloc(BUFSIZE);
    memcpy(lineBuffer, "hello world \0", 13);
    printf("line buffer: %s\n", lineBuffer);
    int lstart, bufsize = BUFSIZE;
    lineSize = BUFSIZE;
    if (!lineBuffer) // Buffer Allocation Failed
	{
		printf("\nBuffer Allocation Error.");
		exit(EXIT_FAILURE);
	}
    while((bytes = read(fd, buffer, bufsize)) > 0) 
    {
        //look for end of line
        lstart = 0;
        for(linePos = 0; linePos < bytes; linePos++){
            if(lineBuffer[linePos] == '\n') {
                int thisLen = linePos - lstart + 1;
                linePos = append(&lineBuffer, buffer + lstart, thisLen, linePos, lineSize);
				printf("next line\n");
				dumpLine(lineBuffer, linePos);
                linePos = 0;
                lstart = linePos + 1;
                //tokenize
                //execute program
            }
        }
        //buffer didn't 
        if(lstart < bytes) {
            // partial line at the end of the buffer
            int thisLen = linePos - lstart;
            linePos = append(&lineBuffer, buffer + lstart, thisLen, linePos, lineSize);
        }
    }
    if(linePos > 0) {
        //file ended with partial line
        append(&lineBuffer, "\n", 1, linePos, lineSize);
		printf("next line\n");
        dumpLine(lineBuffer, linePos);
    }

    printf("done\n");
    free(lineBuffer);
    close(fd);
    return 1;   
}

int append(char **lineBuffer, char *buf, int len, int linePos, int lineSize)
{
    int newPos = linePos + len;
    if(newPos > lineSize) {
        lineSize *= 2;
        assert(lineSize >= newPos);
        *lineBuffer = realloc(*lineBuffer, lineSize);
        //error couldn't allocate more memory
        if (*lineBuffer == NULL) {
            perror("line buffer");
            exit(EXIT_FAILURE);
        }
        
    }
    memcpy(*lineBuffer + linePos, buf, len);
    linePos = newPos;
    return linePos;
}

/**
//call this to get the next on the current lineBuffer
char *next_word(void)
{
	// skip whitespace
	while (1) {
		// ensure we have a char to read
		
		if (!isspace(lineBuffer[pos])) break;
		
		++pos;
	}
	
	// start reading a word
	int start = pos;
	char *word = malloc(1);
	int wordlen = 0;
	do {
		++pos;
		
		if (pos == bytes) {
			// save word so far
			int fraglen = pos - start;
			word = realloc(word, wordlen + fraglen + 1);
			memcpy(word + wordlen, lineBuffer + start, fraglen);
			wordlen += fraglen;
		
			pos = 0;
			start = 0;
		}
		
	} while (!isspace(lineBuffer[pos]));
	
	if (word) {
		word[wordlen] = '\0';
	}
	return word;
}
*/

// requires:
// - linePos is the length of the line in lineBuffer
// - linePos is at least 1
// - final character of current line is '\n'

// this will parse line into tokens and return an array of strings 
void dumpLine(char *lineBuffer, int linePos)
{
	//check that last value of string is new line
	//assert(lineBuffer[linePos-1] == '\n');

	//array of strings representing the tokens
	//the first token should be the command that needs to be run 
	// - a built-in
	// - a bare-name
	
	//should be arraylist
	//char *tokens[];

	//code to parse line
	char *word;
    /**
    	while((word = next_word())){
		printf("next word: %s\n", word);
		free(word);
	}
	free(word);
    */

    //printf("%s \n", lineBuffer);
    printf("%c\n", lineBuffer[0]);
	// dump output to stdout
	write(1, lineBuffer, linePos);
	// FIXME should confirm that all bytes were written
}