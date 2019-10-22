#include "types.h"
#include "user.h"    // getpinfo(), exit()
#include "pstat.h"

int
main(int argc, char *argv[])
{
    struct pstat p;
    if (getpinfo(&p) < 0) {
        printf(2, "getpinfo() error.");
        exit();
    }

    for (int i = 0; i < NPROC; i++) {
        if (p.inuse[i])
            printf(1, "pid: %d  tickets: %d  ticks: %d\n", p.pid[i], p.tickets[i], p.ticks[i]);
    } 
    
    exit();
}