#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#include "words.h"
#include "readline.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFSIZE
#define BUFSIZE 512
#endif

char *lineBuffer;
int linePos, lineSize;

//run interactive mode
int interactiveMode() {
    int fin, bytes, pos, lstart;
    char buffer[BUFSIZE];
    fin = STDIN_FILENO;

    // set up storage for the current line
    lineBuffer = malloc(BUFSIZE + 1);
    lineSize = BUFSIZE;
    linePos = 0;

    // read input
    while ((bytes = read(fin, buffer, BUFSIZE)) > 0) {
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

    free(lineBuffer);
    close(fin);

    return EXIT_SUCCESS;
}

//run batch mode
int batchMode(char *fileName) {
    int fin, bytes, pos, lstart;
    char buffer[BUFSIZE];
    fin = open(fileName, O_RDONLY);
    // set up storage for the current line
    lineBuffer = malloc(BUFSIZE + 1);
    lineSize = BUFSIZE;
    linePos = 0;

    // read input
    while ((bytes = read(fin, buffer, BUFSIZE)) > 0) {
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
void dumpLine(void)
{
    assert(lineBuffer[linePos-1] == '\n');

    words_init(lineBuffer, linePos);

    printf("end of line: %d\n", linePos);

    write(1, lineBuffer, linePos);

    char *word;
    while ((word = words_next())) {
        puts(word);
        free(word);
    }

    // dump output to stdout
    // FIXME should confirm that all bytes were written
}