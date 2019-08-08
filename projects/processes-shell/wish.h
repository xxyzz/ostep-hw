#include <unistd.h>
#include <string.h>

#define INTERACTIVE_MODE 1
#define BATH_MODE 2
#define BUFF_SIZE 256

void
printError()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

void
preExit(char *line, FILE *in)
{
    free(line);
    fclose(in);
}
