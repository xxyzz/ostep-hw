#include <stdio.h>
#include <stdlib.h>      // exit
#include <sys/types.h>
#include <unistd.h>      // fork, exec, access, exit, chdir
#include <sys/wait.h>    // wait
#include "wish.h"
#include <regex.h>

int
parseInput(char *line, ssize_t nread, char *args[], int *args_num, FILE **output)
{
    // remove newline
    if (line[nread - 1] == '\n')
        line[nread - 1] = '\0';

    char *command = strsep(&line, ">");
    if (line != NULL)
    {
        // contain white space in the middle or ">"
        regex_t preg;
        if (regcomp(&preg, "\\S\\s+\\S", REG_EXTENDED) != 0)
        {
            printError();
            return -1;
        }
        if (regexec(&preg, line, 0, NULL, 0) == 0 || strstr(line, ">") != NULL)
        {
            printError();
            return -1;
        }

        regfree(&preg);

        if ((*output = fopen(line, "w")) == NULL)
        {
            printError();
            return -1;
        }
    }

    while ((args[*args_num] = strsep(&command, " ")) != NULL)
        (*args_num)++;

    return 0;
}

int
searchPath(char *paths[], char **path, char *firstArg)
{
    // search executable file in path
    int i = 0;
    while (paths[i] != NULL)
    {
        *path = strdup(paths[i]);
        strcat(*path, "/");
        if (access(strcat(*path, firstArg), X_OK) == 0)
        {
            firstArg = strdup(*path);
            return 0;
        }
        i++;
    }
    return -1;
}

void
redirect(FILE *out)
{
    int outFileno;
    if ((outFileno = fileno(out)) == -1)
    {
        printError();
        return;
    }

    if (outFileno != STDOUT_FILENO)
    {
        // redirect output
        if (dup2(outFileno, STDOUT_FILENO) == -1)
        {
            printError();
            return;
        }
        if (dup2(outFileno, STDERR_FILENO) == -1)
        {
            printError();
            return;
        }
        fclose(out);
    }
}

void
executeCommands(char *args[], int args_num, char *paths[], char *line, FILE *in, FILE *out)
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
        paths[0] = NULL;
        for ( ; i < args_num - 1; i++)
            paths[i] = strdup(args[i+1]);

        paths[i+1] = NULL;
    }
    else
    {
        // not built-in commands
        char *path = "";
        if (searchPath(paths, &path, args[0]) == 0)
        {
            pid_t pid = fork();
            if (pid == -1)
                printError();
            else if (pid == 0)
            {
                // child process
                redirect(out);

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

    while (1)
    {
        if (mode == INTERACTIVE_MODE)
            printf("wish> ");

        if ((nread = getline(&line, &linecap, in)) > 0)
        {
            char *args[BUFF_SIZE];
            int args_num = 0;
            FILE *output = stdout;
            if (parseInput(line, nread, args, &args_num, &output) == 0)
                executeCommands(args, args_num, paths, line, in, output);
        }
        else if (feof(in) != 0)
        {
            preExit(line, in);
            exit(EXIT_SUCCESS);    // EOF
        }
    }

    return 0;
}
