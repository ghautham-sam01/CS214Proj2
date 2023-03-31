#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>

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
	if(DEBUG) {

	}
	int i;
	char *word;
	int pos = 0;
	int listSize = LISTSIZE;
	char **words = malloc(sizeof(char*) * listSize);
	while((word = words_next())) {
		//if the list isn't big enough reallocate

		if(pos >= LISTSIZE) {
			listSize ++;
			words = realloc(words, sizeof(char *) * listSize);
		}

		//check if beginning of first word has ~/ if so replace with /common/home/ks1507
		if((word[0] == '~') && (word[1] == '/')) {
			//replace ~/ with /common/home/ks1507/214 or getenv("HOME");
			int dlen = strlen(word + 2);
			char *home = getenv("HOME");
			int flen = strlen(home);
    		char *pname = malloc(dlen + flen + 2);
    		memcpy(pname, home, flen);
    		pname[flen] = '/';
    		memcpy(pname + flen + 1, word + 2, dlen);
    		pname[dlen + 1 + flen] = '\0';

			word = realloc(word, strlen(pname) + 1);

			strcpy(word, pname);
			free(pname);
		} 
		char *result;
		int last = -1, before, after;
		//check if word has a wildcard "*" in it 
		//if so need to do wildcard expansion
		for(i = 0; i < strlen(word); i++) {
			if(word[i] == '/') {
				last = i;
			}
			else if(word[i] == '*') {
				//check if there's a character after it
				if(word[i + 1] == '\0') {
					printf("error invalid wildcard\n");
					break;
				}
				before = i - 1;
				after = i + 1;
				// do wildcard expansion here
				//barename case look in current directory
				if(last == -1) { 
					result = wildcard(".", word, before, after);
					word = realloc(word, strlen(result) + 1);

					strcpy(word, result);
					free(result);
				}
				//split string from last occurence of "/"
				else {
					before = before - last - 1;
					after = after - last - 1;
					char *path = malloc(last + 1);
					strncpy(path, word, last + 1);
					path[last] = '\0';


					result = wildcard(path, word + last + 1, before, after);
					
					if(!result) { 
						free(word);
						word = NULL;
					} else {
						word = realloc(word, strlen(result) + 1);

						strcpy(word, result);
					}
					free(result);
					free(path); 
				}
			}
		}


		words[pos] = word;
		currSize++;
		pos++;
	}

	free(words[currSize - 1]);
	words[currSize - 1] = NULL;

	return words;
}

char *wildcard(char *dname, char *word, int before, int after) { 
	//we know word contains a * need to split it up into the string before the * and the string after the *

	//example
	//foo*bar
	//before = 2
	//after = 4

	//make substring for before 
	//check beginning of word up until before + 1
	//int is_before = strncmp(word, target, before + 1);
	//check from after up until word - after - 1
	//int is_after = strncmp(word + after, target, (strlen(word - after - 1)));

	//matches both pattern before and after then we know it is a match

	//traversal code
	struct dirent *de;
    long offset;
    int flen;
    int dlen = strlen(dname);
	int last = 0;
	int is_before = 0;
	int is_after = 0;
    char *pname;
    DIR *dp = opendir(dname);
    if (!dp) {
        perror(dname);
        return NULL;
    }

    //check if target exists in this directory
    if (access(pname, F_OK) == 0) {
        //exists in current directory
        return pname;
    } 
    while ((de = readdir(dp))) {
        if (de->d_type != DT_DIR) {
			for(int i = 0; i < strlen(de->d_name); i++){
				if(de->d_name[i] == '/') {
					last = i;
				}
			}

			//de->d_name + last is the remaining part of the string after last '/'
			//check beginning of word up until before + 1
			is_before = strncmp(de->d_name + last, word, before);
			//check from after up until word - after - 1

			is_after = strncmp(de->d_name + (strlen(de->d_name) - (after - 1)), word + after, after - 1);

			if(!is_before && !is_after) { 
				char *ret = malloc(strlen(de->d_name) + 1);
				strcpy(ret, de->d_name);
				return ret;
			}
        }
    }   
    closedir(dp);
    return NULL;

}

char **get_args(char **tokens) {

	//go through tokens list and create a list of arguments
	//all tokens which aren't in the delimiters list {">", "<", "|"}
	char **args = malloc(sizeof(char *) * currSize);
	int i, index = 0;
	for(i = 0; i < (currSize - 1); i++) {	
		char *curr = malloc(sizeof(char) * (strlen(tokens[i]) + 1));
		strcpy(curr, tokens[i]);
		if((strcmp(tokens[i], ">") == 0) || (strcmp(tokens[i], "<") == 0) || strcmp(tokens[i], "|") == 0) {
			i ++;
		} else {
			args[index] = curr;
			index++;
		}
	}
	args[index] = NULL;
	return args;
}

void freeAll(char **tokens, char **args){
	int pos = 0;
	while(tokens[pos] != NULL) {
		if((strcmp(tokens[pos], "<") != 0) && (strcmp(tokens[pos], ">") != 0) && (strcmp(tokens[pos], "|") != 0) && (strcmp(tokens[pos], "\0") != 0)) {
			free(tokens[pos]);
		}
		pos++;
	}

	pos = 0;
	while(args[pos] != NULL) {
		free(args[pos]);
		pos++;
	}

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
	
	return word;
}