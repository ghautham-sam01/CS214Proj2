#include <stdio.h>
#include <stdlib.h>
#include "arraylist.h"

int main(int argc, char **argv){
    printf("Hello! Welcome to our shell\n");
    if(argc==1){
    //Interactive mode
        printf("\nType some commands to begin\n");        
        interactiveMode();
    }else if(argc==2){
    //Batch mode
        printf("\nProcessing file....");
        batchMode(argv[1]);
    }else{
        printf("\n Too many args");
    }


    return 0;
}

int interactiveMode(){
    char *line;
    char **args;
    while(line !='q'){
        printf("myshell> ");
        line = readLine();
        args = getTokens(line);
        execShell(args);
        free(line);
        free(args);
    }
}

int batchMode(){
   return 1;   
}

char *readLine()
{
	char *line = (char *)malloc(sizeof(char) * 1024); // Dynamically Allocate Buffer
	char c;
	int pos = 0, bufsize = 1024;
	if (!line) // Buffer Allocation Failed
	{
		printf("\nBuffer Allocation Error.");
		exit(EXIT_FAILURE);
	}
	while(1)
	{
		c=getchar();
		if (c == EOF || c == '\n') // If End of File or New line, replace with Null character
		{
			line[pos] = '\0';
			return line;
		}
		else
		{
			line[pos] = c;
		}
		pos ++;
		// If we have exceeded the buffer
		if (pos >= bufsize)
		{
			bufsize += 1024;
			line = realloc(line, sizeof(char) * bufsize);
			if (!line) // Buffer Allocation Failed
			{
			printf("\nBuffer Allocation Error.");
			exit(EXIT_FAILURE);
			}
		}
	}
}
char **getTokens(char *line)
{
	char **tokens = (char **)malloc(sizeof(char *) * 64);
	char *token;
	char delim[10] = " \t\n\r\a";
	int pos = 0, bufsize = 64;
	if (!tokens)
	{
		printf("\nBuffer Allocation Error.");
		exit(EXIT_FAILURE);
	}
	token = strtok(line, delim);
	while (token != NULL)
	{
		tokens[pos] = token;
		pos ++;
		if (pos >= bufsize)
		{
			bufsize += 64;
			line = realloc(line, bufsize * sizeof(char *));
			if (!line) // Buffer Allocation Failed
			{
			printf("\nBuffer Allocation Error.");
			exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, delim);
	}
	tokens[pos] = NULL;
	return tokens;
}
// Section Dealing with Built-in Commands

// Function Declarations
int myShell_cd(char **args);
int myShell_exit();

// Definitions
char *builtin_cmd[] = {"cd", "exit"};

int (*builtin_func[]) (char **) = {&myShell_cd, &myShell_exit}; // Array of function pointers for call from execShell

int numBuiltin() // Function to return number of builtin commands
{
	return sizeof(builtin_cmd)/sizeof(char *);
}

// Builtin command definitions
int myShell_cd(char **args)
{
	if (args[1] == NULL) 
	{
		printf("myShell: expected argument to \"cd\"\n");
	} 
	else 
	{
		if (chdir(args[1]) != 0) 
		{
			perror("myShell: ");
		}
	}
	return 1;
}

int myShell_exit()
{
	QUIT = 1;
	return 0;
}

// Function to create child process and run command
int myShellLaunch(char **args)
{
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
	do 
	{
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

// Function to execute command from terminal
int execShell(char **args)
{
	int ret;
	if (args[0] == NULL)
	{
		// Empty command
		return 1;
	}
	// Loop to check for builtin functions
	for (int i=0; i< numBuiltin(); i++) // numBuiltin() returns the number of builtin functions
	{
		if(strcmp(args[0], builtin_cmd[i])==0) // Check if user function matches builtin function name
			return (*builtin_func[i])(args); // Call respective builtin function with arguments
	}
	ret = myShellLaunch(args);
	return ret;
}