#include "myshell.h"
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    //spawn child process for test case
    //child process will execute desired test batch file
    
    int test = argc > 1 ? atoi(argv[1]): 0;
    int pid;
    switch(test) {
    default:
        puts("Missing or invalid test number");
        return EXIT_FAILURE;

    case 1: 
        pid = fork();
        if (pid < 0) { 
            perror("A fork error has occured");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            //in the child process
            execlp("./myshell", "myshell", "test1.sh", NULL);
            printf("execlp didn't work :( \n");
        }
        //clean up child process
        else {
            wait(NULL);
        }
    

    case 2: 
        pid = fork();
        if (pid < 0) { 
            perror("A fork error has occured");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            //in the child process
            execlp("./myshell", "myshell", "test2.sh", NULL);
            printf("execlp didn't work :( \n");
        }
        //clean up child process
        else {
            wait(NULL);
        }

    case 3: 
        pid = fork();
        if (pid < 0) { 
            perror("A fork error has occured");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            //in the child process
            char *args[] = {"echo", "hello world", NULL};
            execvp("/usr/bin/ls", args);
            printf("execvp didn't work :( \n");
        }
        //clean up child process
        else {
            wait(NULL);
        }
    
    //execute test3.sh etc etc

    }

    return EXIT_SUCCESS;
}