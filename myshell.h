int interactiveMode();
int batchMode(char filename[100]);
char *readLine();
char **getTokens(char *line);
int myShell_cd(char **args);
int myShell_exit();
int numBuiltin();
int myShellLaunch(char **args);
int execShell(char **args);