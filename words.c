#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "words.h"

#ifndef BUFSIZE
#define BUFSIZE 128
#endif

#ifndef LISTSIZE
#define LISTSIZE 10
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

static char *buf;
static unsigned pos;
static unsigned bytes;
static int closed;
static int currSize;
char *delimiters[] = {"\0", "\n", "\r"};

//takes char *buffer and size of buffer as input
//currently parses separated by spaces need to account for special characters <, >, | 
void words_init(char *buffer, int size)
{
	buf = buffer;
	pos = 0;
	bytes = size;
	closed = 0;
	currSize = 0;
}

int numDelimiters() { 
	return sizeof(delimiters)/sizeof(char *);
}

char **get_args(void) {
	if(DEBUG) {
		printf("buffer: %s\n", buf);
		printf("bytes: %d\n", bytes);
	}
	int i;
	int isDelim = 0;
	char *word;
	int pos = 0;
	int listSize = LISTSIZE;
	char **words = malloc(sizeof(char*) * listSize);
	while((word = words_next())) {
		//if the list isn't big enough reallocate
		//printf("word: %s\n", word);
		if(pos >= LISTSIZE) {
			listSize *= 2;
			words = realloc(words, sizeof(char *) * listSize);
		}
		/**
		//check if current word isn't a delimiter
		for(i = 0; i < numDelimiters(); i++) {
			if(strcmp(word, "\0") == 0) {
				isDelim = 1;
			}
		}
		if(!isDelim){
			words[pos] = word;
		}
		*/
		words[pos] = word;
		currSize++;
		pos++;
		isDelim = 0;
		//printf("words[pos]=%s\n", words[pos]);
		//free(word);
		//printf("words[pos]=%s\n", words[pos]);
	}

	printf("currSize: %d\n", currSize);
	words[currSize - 1] = NULL;

	if(DEBUG) { 
		//debug check the contents of words array
		for(i = 0; i < currSize; i++){
			printf("element %d: %s\n", i, words[i]);
		}
	}

	//printf("done!\n");
	return words;
}

void freeAll(char **list){
	for(int i = 0; i < currSize; i++) {
		free(list[i]);
	}
}


char *words_next(void)
{
	//printf("buf: %s\n", buf);
	//printf("bytes: %d\n", bytes);
	//printf("pos: %d\n", pos);
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
		
	} while (!isspace(buf[pos]) && (pos < bytes));
	
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