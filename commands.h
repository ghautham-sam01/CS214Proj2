#ifndef _COMMANDS_H_
#define _COMMANDS_H_
int numBuiltin();
int myShell_cd(char **args);
int myShell_pwd(char **args);
int myShell_exit();
int myShellLaunch(char **args);
int execShell(char **args);
char *traverse(char *, char *);
#endif