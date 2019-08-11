#include <stdio.h>
#include <errno.h>
#include <stdlib.h>    // exit
#include <string.h>    // strerror

#define BUFFER_SIZE 1024

int 
main(int argc, char *argv[])
{
    FILE *fp;
    char buff[BUFFER_SIZE];

    for (size_t i = 1; i < argc; i++)
    {
        if ((fp = fopen(argv[i], "r")) == NULL)
        {
            printf("wcat: cannot open file\n");
            exit(EXIT_FAILURE);
        }

        while (fgets(buff, BUFFER_SIZE, fp))
            printf("%s", buff);

        errno = 0;
        if (fclose(fp) != 0) {
            strerror(errno);
            exit(EXIT_FAILURE);
        }
    }

    fclose(fp);

    return 0;
}
