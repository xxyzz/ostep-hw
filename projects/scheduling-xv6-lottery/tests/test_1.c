#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[]) {
    int rc1 = fork();

    if (rc1 == 0) {
        settickets(30);
        sleep(100);
    } else if (rc1 > 0) {
        int rc2 = fork();
        if (rc2 == 0) {
            settickets(20);
            sleep(100);
        } else if (rc2 > 0) {
            int rc3 = fork();
            if (rc3 == 0) {
                settickets(10);
                sleep(100);
            }
        }
    }
    
    for (int i = 0; i < 3; i++) {
        wait();
    }

    exit();
}
