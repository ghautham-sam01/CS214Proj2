#ifndef _READLINE_H_
#define _READLINE_H_
void append(char *, int);
char *readLine(int);
void dumpLine(void);
int interactiveMode();
int batchMode(char *);
#endif