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
        line
    }
}

int batchMode(){
      
}