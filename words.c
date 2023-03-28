#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "words.h"

#ifndef BUFSIZE
#define BUFSIZE 128
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

static char *buf;
static unsigned pos;
static unsigned bytes;
static int closed;

void words_init(char *buffer, int size)
{
	buf = buffer;
	pos = 0;
	bytes = size;
	closed = 0;
}


char *words_next(void)
{
	if (closed) return NULL;
	if (pos > bytes) return NULL;

	// skip whitespace
	while (1) {		
		if (!isspace(buf[pos])) break;
		
		++pos;
	}
	
	// start reading a word
	int start = pos;
	char *word = NULL;
	int wordlen = 0;
	do {
		//continue until you find a space
		++pos;
		
	} while (!isspace(buf[pos]) && (pos < bytes -1));
	
	// grab the word from the current buffer
	// (Note: start == pos if we refreshed the buffer and got a space first.)
	if(DEBUG){
		printf("pos: %d\n", pos);
		printf("start: %d\n", start);
		printf("wordlen: %d\n", wordlen);
	}

	
	if (start < pos) {
		int fraglen = pos - start;
		
		if (DEBUG) printf("fraglen: %d\n", fraglen);
		word = realloc(word, wordlen + fraglen + 1);
		memcpy(word + wordlen, buf + start, fraglen);
		wordlen += fraglen;
	}

	
	if (word) {
		word[wordlen] = '\0';
	}

	if (DEBUG) printf("word: %s\n", word);
	
	return word;
}