#include <stdio.h>
#include <stdlib.h>      // exit
#include <sys/types.h>
#include <unistd.h>      // fork, exec, access, _exit
#include <sys/wait.h>    // wait
#include "wish.h"

int
main(int argc, char *argv[])
{
    int mode = INTERACTIVE_MODE;
    FILE *in = NULL;
    in = stdin;
    char *line = NULL;
    size_t linecap = 0;
    ssize_t nread;
    pid_t pid;
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
            // remove newline
            if (line[nread - 1] == '\n')
                line[nread - 1] = '\0';

            char *args[BUFF_SIZE];
            int args_num = 0;
            while ((args[args_num] = strsep(&line, " ")) != NULL)
                args_num++;

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
            else if (strcmp(args[0], "path") == 0)
            {
                size_t i = 0;
                for ( ; i < args_num; i++)
                    paths[i] = args[i+1];

                paths[i+1] = NULL;
            }
            else
            {
                // find in path
                int i = 0, found = 0;
                char path[BUFF_SIZE] = "";
                while (paths[i] != NULL)
                {
                    strcpy(path, paths[i]);
                    strcat(path, "/");
                    if (access(strcat(path, args[0]), X_OK) == 0)
                    {
                        args[0] = path;
                        found = 1;
                        break;
                    }
                    i++;
                }

                if (!found)
                {
                    printError();
                    continue;
                }

                pid = fork();
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
        }
        else if (feof(in) != 0)
        {
            preExit(line, in);
            exit(EXIT_SUCCESS);    // EOF
        }
    }

    return 0;
}
