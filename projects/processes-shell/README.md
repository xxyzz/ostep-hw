Compile:

```
$ gcc wish.c -o wish -g -Wall -Werror -pthread 
```

Debug child process in GDB:

```
set follow-fork-mode child
```
