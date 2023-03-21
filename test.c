#include myshell.h
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
        if (pid == 0) {
            //in the child process
            execl('myshell.c', 'myshell.c', test1.sh, NULL)
        }
        //clean up child process
        wait(NULL);
    

    case 2: 
        pid = fork();
        if (pid == 0) {
            //in the child process
            execl('myshell.c', 'myshell.c', test2.sh, NULL)
        }
        //clean up child process
        wait(NULL);
    
    //execute test3.sh etc etc

    }

    return EXIT_SUCCESS;
}