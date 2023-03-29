# CS214Proj2

GROUP MEMBERS: Kevin Su (ks1507), Ghautham Sambabu (gs878)


# General testing Plan:

- use many different *.sh files with the naming pattern test<x>.sh and run them in one file test.c
- depending on what argument test.c is passed it will execute the command found in the corresponding *.sh file

In order for our program to be considered correct it needs to satisfy the following conditions:

1. It should be able to run the built-in commands pwd, exit, and cd 
    - if there is an error, mysh should print an error message to standard error and set the last exit status to 0
    - the next command prompt after an error should be "!mysh >" instead of "mysh >"
2. It should create a subprocess when passed a pathname to an executable
    - if the specified program cannot be executed mysh should print an error message and set the last exit status to 0
3. It should create a subprocess when passed a bare name by looking through certain directories
    - if the bare name doesn't exist in any of the directiories it should set the last exit status to 0
    - ow the program should execute the command given by the bare name with the given arguments
    - Our program should search through the following directories: 
        1. /usr/local/sbin
        2. /usr/local/bin
        3. /usr/sbin
        4. /usr/bin
        5. /sbin
        6. /bin
4. It should be able to handle wildcards
    - if the name contains a * symbol the executed command should be run with all files that match the pattern of having the string before the * and end with the string after the *
5. It should be able to handle pipes
6. It should be able to handle extension 3.2 Home Directories
    - When cd is called with no other arguments users should be brought back to the home directory obtained by getenv(HOME)
    - When a pathname starts with ~/ it is starting from the home directory.
7. It should be able to handle extension 3.5 Multiple Pipes 
    - Our shell should be able to handle an arbitrary number of pipes between commands 
    - for example foo | bar | baz | bap should send output from foo -> bar -> baz -> bap
    
Test cases:
    
1. In order to check property 1 we will use batchmode to run a file with the following contents
    
    ```
    pwd
    cd foo
    exit
    ```
    
   If there are no 
    
2. In order to check property 2 we will use batchmode to run a file with the following contents
    
3. In order to check property 3 we will use batchmode to run a file with the following contents
    
4. In order to check property 4 we will use batchmode to run a file with the following contents    
    
5. In order to check property 5 we will use batchmode to run a file with the following contents    

6. In order to check property 6 we will use batchmode to run a file with the following contents    
    
7. In order to check property 7 we will use batchmode to run a file with the following contents    
    

    
# How to run code

First need to build executable files:

```
make myshell
make test1
make test2
...
```

To run test cases (./[test-executable] [test-number])
    
Example to run test case 1:

```
./test 1
```

To start interactive mode: 
```
./myshell
```

# Proof of passing required properties
    
test case 1.)

```
echo foo bar > baz
echo foo > baz bar
echo > baz foo bar
```

test case 2.)
test case 3.)
test case 4.)
                
