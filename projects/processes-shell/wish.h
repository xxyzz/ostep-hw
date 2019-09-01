#include <unistd.h>
#include <string.h>    // strlen, strsep, strcat, strdup, strcmp

#define INTERACTIVE_MODE 1
#define BATH_MODE 2
#define BUFF_SIZE 256

struct function_args
{
    pthread_t thread;
    char *command;
};

void
printError()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

void *
parseInput(void *arg);

int
searchPath(char path[], char *firstArg);

void
redirect(FILE *out);

void
executeCommands(char *args[], int args_num, FILE *out);

char *
trim(char *);

void
clean(void);
