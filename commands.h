#ifndef _COMMANDS_H_
#define _COMMANDS_H_
int numBuiltin();
int myShell_cd(char **tokens, char **args);
int myShell_pwd(char **tokens, char **args);
int myShell_exit();
int myShellLaunch(char **, char **);
int execShell(char **, char **);
char *traverse(char *, char *);
#endif