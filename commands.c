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
#include "words.h"

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
	char *home = getenv("HOME");

	//no args passed
	if (args[1] == NULL) {
		chdir(home);
		return 0; 
	}
	if (chdir(args[1]) != 0) 
	{
		perror("myShell_cd");
		return 1;
	}
	return 0;
}

//only pwd writes to STDOUT so need to check for redirection here

int myShell_pwd(char **tokens, char **args){
	char buf[256];
	int output_redirect = 0;
	int out_fd;
	char *out_file;

	for(int i = 0; tokens[i] != NULL; i++) {
		//this should be allowed
		if(strcmp(tokens[i], ">") == 0) { 
			output_redirect = 1;
			if (tokens[i + 1] == NULL) {
				printf("redirection error: no specified file\n");
				return 1;
			}
			out_file = tokens[i+1];
			break;
		}
		if(strcmp(tokens[i], "|") == 0) { 
			output_redirect = 1;
			if (tokens[i + 1] == NULL) {
				printf("redirection error: no specified file\n");
				return 1;
			}
			out_file = tokens[i+1];
			break;
		}
		//this shouldn't be allowed
		//else if(strcmp() == 0) { 
		//
		//}
	}
	if(getcwd(buf, sizeof(buf)) == NULL) {
		perror("myShell_pwd");
		//close(out_fd);
		return 1;
	}
	else if(output_redirect) {
		out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP);
		if (out_fd == -1) {
			perror("myShell: ");
			return 1;
		}
		write(out_fd, buf, strlen(buf));
	}
	else {
		printf("%s\n", buf);
	}
	close(out_fd);
	return 0;
}

int myShell_exit(){
	printf("mysh: exiting\n");
	exit(EXIT_SUCCESS);
	return 0;
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
	int nPipes = 0, output_redirect = 0, input_redirect = 0, path_name = 0, in_fd = STDIN_FILENO, out_fd = STDOUT_FILENO, i;

	char *in_file=NULL,*out_file = NULL;
	
	//check if there's a slash in the first token
	for (i = 0; i < strlen(tokens[0]) + 1; i++) {
		if(tokens[0][i] == '/') {
			path_name = 1;
			break;
		}
	}

	for (i = 0; tokens[i] != NULL; i++) {
		//printf("tokens[i]: %s\n", tokens[i]);
		if (strcmp(tokens[i],">")==0) {
			output_redirect = 1;
			// if arg after the redirection is NULL we have reached end of arg list
			// display perror and return 0
			if (tokens[i + 1] == NULL) {
				printf("redirection error: no specified file\n");
				return 1;
			}
			out_file = tokens[i+1];
		}
		else if (strcmp(tokens[i],"<")==0) {
			input_redirect = 1;
			// if arg after the redirection is NULL we have reached end of arg list
			// display perror and return 0
			if (tokens[i + 1] == NULL) {
				printf("redirection error: no specified file\n");
				return 1;
			}
			in_file = tokens[i+1];
			args = get_args(tokens);
		}
		if(strcmp(tokens[i],"|")==0){
            nPipes++;
        }
	}
	if(input_redirect){
		in_fd = open(in_file,O_RDONLY);
		if (in_fd == -1) {
			printf("myshell: input file open failed\n");
			return 1;
		}
	}
	if (output_redirect) {
		out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP);
		if (out_fd == -1) {
			perror("myShell: ");
			return 1;
		}
	}

	if (input_redirect) {
		in_fd = open(out_file, O_RDONLY);
		if (out_fd == -1) {
			perror("myShell: ");
			return 1;
		}
	}

	if(nPipes!=0){
        int p[nPipes*2];
        pid_t pid[nPipes+1];
        // handle pipes
        for(i=0;i<nPipes;i++){
        if (pipe(p + i*2) == -1) {
            perror("pipe");
            exit(1);
            }
        }
          /* Fork all the child processes */
        for (int i = 0; i <= nPipes; i++) {
            pid[i] = fork();
            if (pid[i] == -1) {
				perror("fork");
				exit(1);
            } else if (pid[i] == 0) {
				/* Child process: execute the appropriate command */
				if (i == 0) {
					/* First command: read from standard input */
					close(p[0]);
					dup2(p[1], STDOUT_FILENO);
					close(p[1]);
					execvp(args[1], args);
					perror("execvp");
					exit(1);
				} else if (i == nPipes) {
					/* Last command: write to standard output */
					close(p[(i-1)*2]);
					dup2(p[(i-1)*2 + 1], STDOUT_FILENO);
					close(p[(i-1)*2 + 1]);
					execvp(args[i+1], args);
					perror("execvp");
					exit(1);
				} else {
					/* Middle commands: read from previous pipe, write to next pipe */
					close(p[(i-1)*2]);
					dup2(p[(i-1)*2 + 1], STDOUT_FILENO);
					close(p[(i-1)*2 + 1]);
					close(p[i*2 + 1]);
					dup2(p[i*2], STDIN_FILENO);
					close(p[i*2]);
					execvp(args[i+1], args);
					perror("execlp");
					exit(1);
				}
            }
        }
        
        /* Wait for all child processes to complete */
        for (int i = 0; i <= nPipes; i++) {
            waitpid(pid[i],NULL, 0);
        }
    }	
	// ow just run 
	else{
		//printf("normal exec\n");
		pid_t pid, wpid;
		int status;
		char *pname = NULL; 
		//printf("args[0]: %s\n", args[0]);
		if(!path_name) {
			for(int i = 0; i < numRoutes(); i++) {
				pname = traverse(routes[i], args[0]);
				if(pname != NULL) break;
			}
		}

		//what should it do if the pname is not recognized
		//probably throw an error here
		if(pname == NULL && !path_name) {
			perror("myShell");
			//free(pname);
			return 1;
		}
		pid = fork();
		if (pid < 0)
		{
			//Forking Error
			perror("myShell: ");
			return 1;
		}
		else if (pid == 0)
		{	
			//printf("input_redirect: %d\n", input_redirect);
			//printf("output_redirect: %d\n", output_redirect);
			//printf("path_name: %d\n", path_name);
			//printf("pname: %s\n", pname);
			// The Child Process
			if(output_redirect) { 
				//printf("out redirect\n");
				dup2(out_fd, STDOUT_FILENO);
			}

			if(input_redirect) { 
				//printf("in redirect\n");
				dup2(in_fd, STDIN_FILENO);
			}
			if (path_name)
			{
				//printf("path_name\n");
				execv(args[0], args);
				printf("exec failed\n");
				return 1;
			}
			else
			{ 
				//printf("bare_name\n");
				execv(pname, args);
				printf("exec failed\n");
				return 1;
			}
		}
		else
		{
			// The Parent Process
			do {
				wait(NULL);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
		if(input_redirect) {
			close(in_fd);
		}
		if(output_redirect) {
			close(out_fd);
		}
		//printf("done\n");
		free(pname);
	}
	return 0;
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
    } 
    while ((de = readdir(dp))) {
        //printf("%s/%s %d\n", dname, de->d_name, de->d_type);

        if (de->d_type == DT_DIR && de->d_name[0] != '.') {
            // construct new path
            flen = strlen(de->d_name);
            pname = realloc(pname, dlen + flen + 2);
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
	//free(pname);
    closedir(dp);
    return NULL;
}