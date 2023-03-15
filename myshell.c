#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    
    if(argc==1){
    //Interactive mode
        printf("Hello! Welcome to our shell");
        interactiveMode();
    }else if(argc==2){
    //Batch mode
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