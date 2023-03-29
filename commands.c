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
		chdir(getenv("HOME"));
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
	// handle output redirection
	int output_redirect = 0, input_redirect=0 ,in_fd = STDIN_FILENO, out_fd = STDOUT_FILENO, i;

	char *in_file=NULL,*out_file = NULL;
	for (i = 0; args[i] != '\0'; i++) {
		if (strcmp(args[i],">")==0) {
			args[i] = '\0';
			output_redirect = 1;
			out_file = &args[i+1];
			break;
		}
		if (strcmp(args[i],"<")==0) {
			args[i] = '\0';
			input_redirect = 1;
			in_file = &args[i+1];
			break;
		}
	}
	if(input_redirect){
		in_fd = open(in_file,O_RDONLY);
		if (in_fd == -1) {
			printf("myshell: input file open failed\n");
		}
	}
	if (output_redirect) {
		out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC,0644);
		if (out_fd == -1) {
			printf("myshell: output file open failed\n");
		}
	}
	int p[2];
	// handle pipes
	int pipe_index = -1;
	for (i = 0; args[i] != '\0'; i++) {
		if (strcmp(args[i],"|")==0) {
			args[i] = '\0';
			pipe_index = i;
			break;
		}
	}

      if (pipe_index != -1) {
            // create pipe
            if (pipe(p) == -1) {
                printf("myshell: pipe creation failed\n");
            }

            // execute left command
            args[0] = args[pipe_index-1];
            args[1] = NULL;
            pid_t pid_left = fork();
            if (pid_left == 0) {
                dup2(in_fd, STDIN_FILENO);
                close(p[0]); // close unused read end
                dup2(p[1], STDOUT_FILENO);
                close(p[1]); // close write end
                execvp(args[0], args);
                printf("myshell: command not found: %s\n", args[0]);
                exit(0);
            } else if (pid_left < 0) {
                printf("myshell: fork failed\n");
            }

            // execute right command
            args[0] = args[pipe_index+1];
            args[1] = NULL;
            pid_t pid_right = fork();
            if (pid_right == 0) {
                dup2(p[0], STDIN_FILENO);
                close(p[1]); // close unused write end
                dup2(out_fd, STDOUT_FILENO);
                close(p[0]); // close read end
                execvp(args[0], args);
                printf("myshell: command not found: %s\n", args[0]);
                exit(0);
            } else if (pid_right < 0) {
                printf("myshell: fork failed\n");
            }

            // wait for both child processes to finish
            close(p[0]);
            close(p[1]);
            waitpid(pid_left, NULL, 0);
            waitpid(pid_right,NULL,0);
	}else{
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

