#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <wait.h>

#include "commands.h"

// Section Dealing with Built-in Commands

// Definitions
char *builtin_cmd[] = {"cd", "pwd", "exit"};

int (*builtin_func[]) (char **) = {&myShell_cd, &myShell_pwd, &myShell_exit}; // Array of function pointers for call from execShell

int numBuiltin() // Function to return number of builtin commands
{
	return sizeof(builtin_cmd)/sizeof(char *);
}

// Builtin command definitions
int myShell_cd(char **args){
	if (args[1] == NULL || args[1] == '\0' || args[1] == ' ') 
	{
		perror("cd: expected argument to \"cd\"\n");
		exit(EXIT_FAILURE);
	} 
	else 
	{
		if (chdir(args[1]) != 0) 
		{
			perror("cd: No such file or directory");
			exit(EXIT_FAILURE);
		}
	}
	return 1;
}

int myShell_pwd(char **args){
	char buf[256];

	if(getcwd(buf, sizeof(buf)) == NULL) {
		perror("pwd error");
	}
	else {
		printf("%s\n", buf);
	}
	/**
	
	NEED TO IMPLEMENT
	
	*/
	return 1;
}

int myShell_exit(){
	printf("mysh: exiting\n");
	exit(EXIT_SUCCESS);
	return 0;
}

// Function to create child process and run command
int myShellLaunch(char **args){
	pid_t pid, wpid;
	int status;
	pid = fork();
	if (pid == 0)
	{
		// The Child Process
		if (execvp(args[0], args) == -1)
		{
			perror("myShell: ");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		//Forking Error
		perror("myShell: ");
	}
	else
	{
		// The Parent Process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

// Function to execute command from terminal
int execShell(char **args){
	int ret;
	if (args[0] == NULL)
	{
		// Empty command
		return 1;
	}
	// Loop to check for builtin functions
	for (int i=0; i< numBuiltin(); i++) // numBuiltin() returns the number of builtin functions
	{
		if(strcmp(args[0], builtin_cmd[i])==0){ // Check if user function matches builtin function name
			//printf("args[0]: %s\n", args[0]);
			return (*builtin_func[i])(args); // Call respective builtin function with arguments
		}
	}
	ret = myShellLaunch(args);
	return ret;
}

