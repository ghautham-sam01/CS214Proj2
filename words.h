#ifndef _WORDS_H_
#define _WORDS_H_
void words_init(char *, int);
char *words_next(void);
char **get_args(char **);
char **get_tokens(void);
void freeAll(char **, char **);
#endif