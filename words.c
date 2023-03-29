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

//
char **get_tokens(void) {
	printf("get tokens\n");
	if(DEBUG) {
		printf("buffer: %s\n", buf);
		printf("bytes: %d\n", bytes);
	}
	int i;
	char *word;
	int pos = 0;
	int listSize = LISTSIZE;
	char **words = malloc(sizeof(char*) * listSize);
	while((word = words_next())) {
		//if the list isn't big enough reallocate
		//printf("word: %s\n", word);
		if(pos >= LISTSIZE) {
			listSize ++;
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

		//check if word has a wildcard "*" in it 
		//if so need to do wildcard expansion


		words[pos] = word;
		currSize++;
		pos++;
		//isDelim = 0;
		//printf("words[pos]=%s\n", words[pos]);
		//free(word);
		//printf("words[pos]=%s\n", words[pos]);
	}

	//printf("currSize: %d\n", currSize);
	free(words[currSize - 1]);
	words[currSize - 1] = NULL;


	//debug check the contents of words array
	for(i = 0; i < currSize; i++){
		printf("element %d: %s\n", i, words[i]);
	}

	return words;
}

char **get_args(char **tokens) {

	printf("get args\n");
	//go through tokens list and create a list of arguments
	//all tokens which aren't in the delimiters list {">", "<", "|"}
	char **args = malloc(sizeof(char *) * currSize);
	int i, index = 0;
	for(i = 0; i < (currSize - 1); i++) {	
		// check if current string is ">", "<", or "|"
		// if it is don't include this token and the next one in the args list
		char *curr = malloc(sizeof(char *));
		strcpy(curr, tokens[i]);
		if((strcmp(tokens[i], ">") == 0) || (strcmp(tokens[i], "<") == 0) || strcmp(tokens[i], "|") == 0) {
			i ++;
		} else {
			args[index] = curr;
			index++;
		}
	}
	printf("currSize: %d\n", currSize);
	printf("index: %d\n", index);
	args[index] = NULL;

	/**
		for(i = 0; i < (index); i++){
		printf("element %d: %s\n", i, args[i]);
	}

	*/

	printf("done !\n");
	return args;
}

void freeAll(char **tokens, char **args){
	int pos = 0;
	printf("tokens\n");
	while(tokens[pos] != NULL) {
		printf("tokens[pos]: %s\n", tokens[pos]);
		if((strcmp(tokens[pos], "<") != 0) && (strcmp(tokens[pos], ">") != 0) && (strcmp(tokens[pos], "|") != 0) && (strcmp(tokens[pos], "\0") != 0)) {
			free(tokens[pos]);
		}
		pos++;
	}

	pos = 0;
	printf("args\n");
	while(args[pos] != NULL) {
		printf("element %d: %s\n", pos, args[pos]);
		free(args[pos]);
		pos++;
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

	// check for <, >, or |
	if(buf[pos] == '<') {
		pos++;
		return "<";
	} 
	else if(buf[pos] == '>') {
		pos++;
		return ">";
	}
	else if(buf[pos] == '|') {
		pos++;
		return "|";
	}


	do {
		//continue until you find a space
		++pos;
		
	} while (!isspace(buf[pos]) && (pos < bytes) && (buf[pos] != '>') && (buf[pos] != '<') && (buf[pos] != '|'));
	
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