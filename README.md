# CS214Proj2

GROUP MEMBERS: Kevin Su (ks1507), Ghautham Sambabu (gs878)


Testing Plan:

- use many different *.sh files with the naming pattern test<x>.sh and run them in one file test.c
- depending on what argument test.c is passed it will execute the command found in the corresponding *.sh file

Correctness:

In order for our program to be considered correct it needs to satisfy the following conditions

1. It should be able to run the built-in commands pwd and cd 
    - if there is an error, mysh should print an error message to standard error and set the last exit status to 1
2. It should create a subprocess when passed a pathname to an executable
    - if the specified program cannot be executed mysh should print an error message and set the last exit status to 1
3. It should create a subprocess when passed a bare name by looking through certain directories
    - if the 
4. It should be able to handle wildcards
5. It should be able to handle pipes
6. It should be able to handle extension 1
7. It should be able to handle extension 2
8. If passed more than one argument the 