#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <wait.h>
#include <dirent.h>

#include "commands.h"

// Section Dealing with Built-in Commands

// Definitions
char *builtin_cmd[] = {"cd", "pwd", "exit"};

//char *home = getenv("HOME");

char *routes[] = {"/usr/local/sbin", "/usr/local/bin", "/usr/sbin", "/usr/bin", "/sbin", "/bin"};

int (*builtin_func[]) (char **, char **) = {&myShell_cd, &myShell_pwd, &myShell_exit}; // Array of function pointers for call from execShell

int numBuiltin() // Function to return number of builtin commands
{
	return sizeof(builtin_cmd)/sizeof(char *);
}

int numRoutes() // Function to return number of builtin routes to check 
{
	return sizeof(routes)/sizeof(char *);
}

// Builtin command definitions
int myShell_cd(char **tokens, char **args){
	if (args[1] == NULL || (strcmp(args[1], "\0") == 0)) 
	{
		perror("myShell_cd");
		return 0;
	} 
	else 
	{
		//no args passed
		//if (args[1] == NULL) {
		//	chidir(home);
		//	return 1; 
		//}
		if (chdir(args[1]) != 0) 
		{
			perror("myShell_cd");
			return 0;
		}
	}
	return 1;
}

int myShell_pwd(char **tokens, char **args){
	char buf[256];

	if(getcwd(buf, sizeof(buf)) == NULL) {
		perror("myShell_pwd");
		return 0;
	}
	else {
		printf("%s\n", buf);
	}

	return 1;
}

int myShell_exit(){
	printf("mysh: exiting\n");
	exit(EXIT_SUCCESS);
	return 1;
}

// Function to create child process and run command
/**
function requested is not a built-in function 
check through the following directories for a file with the specified name: 

1. /usr/local/sbin
2. /usr/local/bin
3. /usr/sbin
4. /usr/bin
5. /sbin
6. /bin

*/
int myShellLaunch(char **tokens, char **args){
	// handle output redirection
	// '>' redirect output to file specified 
	// should change stdout to the specified file
	// if file exists then overwrite it 
	// ow create a new file
	
	// '<' redirect input to file specified
	// should change stdin to the specified file
	// if file doesn't exist then we should return an error
	// ow proceed normally
	int output_redirect = 0, input_redirect = 0, in_fd = STDIN_FILENO, out_fd = STDOUT_FILENO, i;

	char *in_file=NULL,*out_file = NULL;

	for (i = 0; tokens[i] != NULL; i++) {
		if (strcmp(tokens[i],">")==0) {
			//tokens[i] = "\0";
			output_redirect = 1;
			// if arg after the redirection is NULL we have reached end of arg list
			// display perror and return 0
			if (tokens[i + 1] == NULL) {
				printf("redirection error: no specified file\n");
				return 0;
			}
			out_file = tokens[i+1];
			//tokens[i + 1] = "\0";
			break;
		}
		if (strcmp(tokens[i],"<")==0) {
			//tokens[i] = "\0";
			input_redirect = 1;
			// if arg after the redirection is NULL we have reached end of arg list
			// display perror and return 0
			if (tokens[i + 1] == NULL) {
				printf("redirection error: no specified file\n");
				return 0;
			}
			in_file = tokens[i+1];
			//tokens[i + 1] = "\0";
			args = get_args(tokens);
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
		out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP);
		if (out_fd == -1) {
			perror("myShell: ");
			return 0;
		}
	}

	if (input_redirect) {
		in_fd = open(out_file, O_RDONLY);
		if (out_fd == -1) {
			perror("myShell: ");
			return 0;
		}
	}
	int p[2];
	// handle pipes
	int pipe_index = -1;
	for (i = 0; tokens[i] != '\0'; i++) {
		if (strcmp(tokens[i],"|")==0) {
			tokens[i] = '\0';
			pipe_index = i;
			break;
		}
	}

	//need to pipe
    if (pipe_index != -1) {
			printf("pipe exec");
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
	}
	// ow just run 
	else{
		printf("normal exec\n");
		pid_t pid, wpid;
		int status;
		char *pname = NULL; 
		for(int i = 0; i < numRoutes(); i++) {
			pname = traverse(routes[i], args[0]);
			if(pname != NULL) break;
		}
		//what should it do if the pname is not recognized
		//probably throw an error here
		if(pname == NULL) {
			perror("myShell");
			free(pname);
			return 0;
		}
		pid = fork();
		if (pid == 0)
		{
			// The Child Process
			if(output_redirect) { 
				dup2(out_fd, STDOUT_FILENO);
			}

			if(input_redirect) { 
				dup2(in_fd, STDIN_FILENO);
			}

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
		if(input_redirect) {
			close(in_fd);
		}
		if(output_redirect) {
			close(out_fd);
		}
		free(pname);
	}
	return 1;
}

// Function to execute command from terminal
int execShell(char **tokens, char **args){
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

			return (*builtin_func[i])(tokens, args); // Call respective builtin function with arguments
		}
	}
	ret = myShellLaunch(tokens, args);
	return ret;
}

char *traverse(char *dname, char *target)
{
    struct dirent *de;
    long offset;
    int flen;
    int dlen = strlen(dname);
    char *pname;
    DIR *dp = opendir(dname);
    if (!dp) {
        perror(dname);
        return NULL;
    }

    // construct new path
    flen = strlen(target);
    pname = malloc(dlen + flen + 2);
    memcpy(pname, dname, dlen);
    pname[dlen] = '/';
    memcpy(pname + dlen + 1, target, flen);
    pname[dlen + 1 + flen] = '\0';

    //printf("Traversing %s\n", dname);
    //check if target exists in current directory
    //printf("checking for %s\n", pname);
    if (access(pname, F_OK) == 0) {
        //exists in current directory
        return pname;
        printf("exists !\n");
    } 
    while ((de = readdir(dp))) {
        //printf("%s/%s %d\n", dname, de->d_name, de->d_type);

        if (de->d_type == DT_DIR && de->d_name[0] != '.') {
            // construct new path
            flen = strlen(de->d_name);
            pname = malloc(dlen + flen + 2);
            memcpy(pname, dname, dlen);
            pname[dlen] = '/';
            memcpy(pname + dlen + 1, de->d_name, flen);
            pname[dlen + 1 + flen] = '\0';
            // save location in directory
            offset = telldir(dp);
            closedir(dp);
            // recursively traverse subdirectory
            traverse(pname, target);
            free(pname);
            // restore position in directory
            dp = opendir(dname); // FIXME: check for success
            seekdir(dp, offset);
        }
    }   
    closedir(dp);
    return NULL;
}