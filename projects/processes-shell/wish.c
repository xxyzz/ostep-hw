#include <stdio.h>
#include <stdlib.h>      // exit
#include <sys/types.h>
#include <unistd.h>      // fork, exec, access, exit, chdir
#include <sys/wait.h>    // wait
#include "wish.h"

void
parseInput(char *line, ssize_t nread, char *args[], int *args_num)
{
    // remove newline
    if (line[nread - 1] == '\n')
        line[nread - 1] = '\0';

    while ((args[*args_num] = strsep(&line, " ")) != NULL)
        (*args_num)++;
}

int
searchPath(char *paths[], char path[], char *firstArg)
{
    // search executable file in path
    int i = 0;
    while (paths[i] != NULL)
    {
        strcpy(path, paths[i]);
        strcat(path, "/");
        if (access(strcat(path, firstArg), X_OK) == 0)
        {
            firstArg = path;
            return 1;
        }
        i++;
    }
    return 0;
}

void
executeCommands(char *args[], int args_num, char *paths[], char *line, FILE *in)
{
    // check built-in commands first
    if (strcmp(args[0], "exit") == 0)
    {
        if (args_num > 1)
            printError();
        else
        {
            free(line);
            fclose(in);
            exit(EXIT_SUCCESS);
        }
    }
    else if (strcmp(args[0], "cd") == 0)
    {
        if (args_num == 1 || args_num > 2)
            printError();
        else if (chdir(args[1]) == -1)
            printError();  
    }
    else if (strcmp(args[0], "path") == 0)
    {
        size_t i = 0;
        for ( ; i < args_num; i++)
            paths[i] = args[i+1];

        paths[i+1] = NULL;
    }
    else
    {
        // not built-in commands
        char path[BUFF_SIZE] = "";
        if (searchPath(paths, path, args[0]))
        {
            pid_t pid = fork();
            if (pid == -1)
                printError();
            else if (pid == 0)
            {
                // child process    
                if (execv(path, args) == -1)
                    printError();  
            }
            else
            {
                // parent process
                // wait all children
                while(wait(NULL) != -1)
                    ;
            }
        }
        else
            printError();    // not fond in path
    }
}

int
main(int argc, char *argv[])
{
    int mode = INTERACTIVE_MODE;
    FILE *in = NULL;
    in = stdin;
    char *line = NULL;
    size_t linecap = 0;
    ssize_t nread;
    char *paths[BUFF_SIZE] = {"/bin", NULL};

    if (argc > 1)
    {
        mode = BATH_MODE;
        if (argc > 2 || (in = fopen(argv[1], "r")) == NULL)
        {
            printError();
            exit(EXIT_FAILURE);
        }
    }

    while (1) {
        if (mode == INTERACTIVE_MODE)
            printf("wish> ");

        if ((nread = getline(&line, &linecap, in)) > 0)
        {
            char *args[BUFF_SIZE];
            int args_num = 0;
            parseInput(line, nread, args, &args_num);

            executeCommands(args, args_num, paths, line, in);
        }
        else if (feof(in) != 0)
        {
            preExit(line, in);
            exit(EXIT_SUCCESS);    // EOF
        }
    }

    return 0;
}
