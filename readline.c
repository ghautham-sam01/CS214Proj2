#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#include "words.h"
#include "readline.h"
#include "commands.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFSIZE
#define BUFSIZE 512
#endif

char *lineBuffer;
int linePos, lineSize, fin;

//read next line from stdinput
char *readLine(int fin) { 
    int bytes, pos, lstart;
    char buffer[BUFSIZE];
    //fin = 0;

    // set up storage for the current line
    lineBuffer = malloc(BUFSIZE + 1);
    lineSize = BUFSIZE;
    linePos = 0;
    while(1) {
        bytes = read(fin, buffer, lineSize);
        if (DEBUG) fprintf(stderr, "read %d bytes\n", bytes);
        // search for newlines
        lstart = 0;
        for (pos = 0; pos < bytes; ++pos) {
            if (buffer[pos] == '\n') {
                int thisLen = pos - lstart + 1;
                if (DEBUG) fprintf(stderr, "finished line %d+%d bytes\n", linePos, thisLen);

                append(buffer + lstart, thisLen);
                assert(lineBuffer[linePos-1] == '\n');
                lineBuffer[linePos] = '\0';
                //write(1, lineBuffer, linePos); 
                return lineBuffer;
                linePos = 0;
                lstart = pos + 1;
                
            }
        }
        if (lstart < bytes) {
            // partial line at the end of the buffer
            int thisLen = pos - lstart;
            if (DEBUG) fprintf(stderr, "partial line %d+%d bytes\n", linePos, thisLen);
            append(buffer + lstart, thisLen);
        }
    }
    if (linePos > 0) {
        // file ended with partial line
        append("\n", 1);
        assert(lineBuffer[linePos-1] == '\n');
        lineBuffer[linePos] = '\0';
        //write(1, lineBuffer, linePos);
        return lineBuffer;
    }

    return lineBuffer;
}

//run interactive mode
int interactiveMode() {
    char *line;
    char **args;
    char **tokens;

    //0 for success 1 for failure 
    int status = 0;

    while(1) { 
        //no error
        if(!status) {
            write(1, "mysh> ", 7);
        }
        //there was an error in the previous command
        else {
            write(1, "!mysh> ", 7);
        }

        line = readLine(STDIN_FILENO);
        //do tokenization
        words_init(line, linePos);
        tokens = get_tokens();
        args = get_args(tokens);
        //exec
        status = execShell(tokens, args);
        //printf("execution complete, status: %d\n", status);

        //printf("status: %d\n", status);

        //printf("line: %s\n", line);
        freeAll(tokens, args);
        free(args);
        free(tokens);
        free(line);
        //free(lineBuffer);
    }

    close(0);
    return EXIT_SUCCESS;
}

//run batch mode
int batchMode(char *fileName) {
    printf("BATCH MODE!\n");
    /**
    fin = open(fileName, O_RDONLY);

    if(fin == -1) { 
        perror("batchMode: ");
        return 1;
    }
    char *line ="";
    char **tokens;
    char **args;
    int status;
    while((line = readLine(fin)) != NULL) {
        printf("line: %s\n", line);
        //do tokenization
        words_init(line, linePos);
        tokens = get_tokens();
        args = get_args(tokens);
        //exec
        status = execShell(tokens, args);
        printf("execution complete, status: %d\n", status);

        //printf("status: %d\n", status);

        //printf("line: %s\n", line);
        freeAll(tokens, args);
        free(args);
        free(tokens);
        free(line);
        //free(lineBuffer);
    } 
    printf("done");
    */
    
    int bytes, pos,lstart;
    char *buffer[BUFSIZE];
    printf("BATCH MODE!\n");
    fin = open(fileName, O_RDONLY);

    if(fin == -1) { 
        perror("batchMode: ");
        return 1;
    }

    // set up storage for the current line
    lineBuffer = malloc(BUFSIZE + 1);
    lineSize = BUFSIZE;
    linePos = 0;

    // read input
    while ((bytes = read(fin, buffer, BUFSIZE)) > 0) {
        write(1, lineBuffer, linePos);
        if (DEBUG) fprintf(stderr, "read %d bytes\n", bytes);

        // search for newlines
        lstart = 0;
        for (pos = 0; pos < bytes; ++pos) {
            if (buffer[pos] == '\n') {
                int thisLen = pos - lstart + 1;
                if (DEBUG) fprintf(stderr, "finished line %d+%d bytes\n", linePos, thisLen);

                append(buffer + lstart, thisLen);
                dumpLine();
                linePos = 0;
                lstart = pos + 1;
            }
        }
        if (lstart < bytes) {
            // partial line at the end of the buffer
            int thisLen = pos - lstart;
            if (DEBUG) fprintf(stderr, "partial line %d+%d bytes\n", linePos, thisLen);
            append(buffer + lstart, thisLen);
        }
    }
    if (linePos > 0) {
        // file ended with partial line
        append("\n", 1);
        dumpLine();
    }
    
    printf("done\n");

    free(lineBuffer);
    close(fin);
    
    return EXIT_SUCCESS;
}

// add specified text the line buffer, expanding as necessary
// assumes we are adding at least one byte
void append(char *buf, int len)
{
    int newPos = linePos + len;
    
    if (newPos > lineSize) {
        lineSize *= 2;
        if (DEBUG) fprintf(stderr, "expanding line buffer to %d\n", lineSize);
        assert(lineSize >= newPos);
        lineBuffer = realloc(lineBuffer, lineSize);
        if (lineBuffer == NULL) {
            perror("line buffer");
            exit(EXIT_FAILURE);
        }
    }

    memcpy(lineBuffer + linePos, buf, len);
    linePos = newPos;
}

// print the contents of crntLine in reverse order
// requires: 
// - linePos is the length of the line in lineBuffer
// - linePos is at least 1
// - final character of current line is '\n'

//fix: change this to making an array of strings char **args which will be passed to another function to exec the command on this line
void dumpLine(void)
{
    assert(lineBuffer[linePos-1] == '\n');
    lineBuffer[linePos] = '\0';
    words_init(lineBuffer, linePos);

    //printf("end of line: %d\n", linePos);

    //write(1, lineBuffer, linePos);

    char **tokens = get_tokens();
    char **args = get_args(tokens);
    int status = execShell(tokens, args);

    printf("exec status: %d\n", status);

    freeAll(args, tokens);
    free(args);
    free(tokens);

    // dump output to stdout
    // FIXME should confirm that all bytes were written
}