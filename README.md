# CS214Proj2

GROUP MEMBERS: Kevin Su (ks1507), Ghautham Sambabu (gs878)


General testing Plan:

- use many different *.sh files with the naming pattern test<x>.sh and run them in one file test.c
- depending on what argument test.c is passed it will execute the command found in the corresponding *.sh file

In order for our program to be considered correct it needs to satisfy the following conditions:

1. It should be able to run the built-in commands pwd, exit, and cd 
    - if there is an error, mysh should print an error message to standard error and set the last exit status to 1
    - the next command prompt after an error should be "!mysh >" instead of "mysh >"
2. It should create a subprocess when passed a pathname to an executable
    - if the specified program cannot be executed mysh should print an error message and set the last exit status to 1
3. It should create a subprocess when passed a bare name by looking through certain directories
    - if the bare name doesn't exist it should set the last exit status to 1
4. It should be able to handle wildcards
    - if the name contains a * symbol the executed command should be run with all files that match the pattern of having the string before the * and end with the string after the *
5. It should be able to handle pipes
6. It should be able to handle extension 1
7. It should be able to handle extension 2

# How to run code

First need to build executable files:

```
make myshell
make test1
make test2
...
```

To run test cases (./[test-executable] [test-number])

```
./test1 1
```

# Proof of passing required properties
