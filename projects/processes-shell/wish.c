#include <stdio.h>       // fopen, fclose, fileno, getline, feof
#include <stdlib.h>      // exit
#include <sys/types.h>
#include <unistd.h>      // fork, exec, access, exit, chdir
#include <sys/wait.h>    // waitpid
#include "wish.h"
#include <regex.h>       // regcomp, regexec, regfree
#include <pthread.h>     // pthread_create, pthread_join
#include <ctype.h>       // isspace

FILE *in = NULL;
char *paths[BUFF_SIZE] = {"/bin", NULL};
char *line = NULL;

void
clean(void)
{
    free(line);
    fclose(in);
}

char *
trim(char *s)
{
    // trim leading spaces
    while (isspace(*s))
        s++;

    if (*s == '\0')
        return s;    // empty string

    // trim trailing spaces
    char *end = s + strlen(s) - 1;
    while (end > s && isspace(*end))
        end--;

    end[1] = '\0';
    return s;
}

void *
parseInput(void *arg)
{
    char *args[BUFF_SIZE];
    int args_num = 0;
    FILE *output = stdout;
    struct function_args *fun_args = (struct function_args *) arg;
    char *commandLine = fun_args->command;

    char *command = strsep(&commandLine, ">");
    if (command == NULL || *command == '\0')
    {
        printError();
        return NULL;
    }

    command = trim(command);

    if (commandLine != NULL)
    {
        // contain white space in the middle or ">"
        regex_t reg;
        if (regcomp(&reg, "\\S\\s+\\S", REG_EXTENDED) != 0)
        {
            printError();
            regfree(&reg);
            return NULL;
        }
        if (regexec(&reg, commandLine, 0, NULL, 0) == 0 || strstr(commandLine, ">") != NULL)
        {
            printError();
            regfree(&reg);
            return NULL;
        }

        regfree(&reg);

        if ((output = fopen(trim(commandLine), "w")) == NULL)
        {
            printError();
            return NULL;
        }
    }

    char **ap = args;
    while ((*ap = strsep(&command, " \t")) != NULL)
        if (**ap != '\0')
        {
            *ap = trim(*ap);
            ap++;
            if (++args_num >= BUFF_SIZE)
                break;
        }

    if (args_num > 0)
        executeCommands(args, args_num, output);

    return NULL;
}

int
searchPath(char path[], char *firstArg)
{
    // search executable file in path
    int i = 0;
    while (paths[i] != NULL)
    {
        snprintf(path, BUFF_SIZE, "%s/%s", paths[i], firstArg);
        if (access(path, X_OK) == 0)
            return 0;
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
executeCommands(char *args[], int args_num, FILE *out)
{
    // check built-in commands first
    if (strcmp(args[0], "exit") == 0)
    {
        if (args_num > 1)
            printError();
        else
        {
            atexit(clean);
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
        char path[BUFF_SIZE];
        if (searchPath(path, args[0]) == 0)
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
                waitpid(pid, NULL, 0);    // parent process waits child
        }
        else
            printError();    // not fond in path
    }
}

int
main(int argc, char *argv[])
{
    int mode = INTERACTIVE_MODE;
    in = stdin;
    size_t linecap = 0;
    ssize_t nread;

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
            char *command;
            int commands_num = 0;
            struct function_args args[BUFF_SIZE];

            // remove newline character
            if (line[nread - 1] == '\n')
                line[nread - 1] = '\0';

            char *temp = line;

            while ((command = strsep(&temp, "&")) != NULL)
                if (command[0] != '\0')
                {
                    args[commands_num++].command = strdup(command);
                    if (commands_num >= BUFF_SIZE)
                        break;
                }

            for (size_t i = 0; i < commands_num; i++)
                if (pthread_create(&args[i].thread, NULL, &parseInput, &args[i]) != 0)
                    printError();

            for (size_t i = 0; i < commands_num; i++)
            {
                if (pthread_join(args[i].thread, NULL) != 0)
                    printError();
                if (args[i].command != NULL)
                    free(args[i].command);
            }
        }
        else if (feof(in) != 0)
        {
            atexit(clean);
            exit(EXIT_SUCCESS);    // EOF
        }
    }

    return 0;
}
