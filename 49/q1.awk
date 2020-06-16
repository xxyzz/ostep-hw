#!/usr/bin/env awk -f

{
    if ( NR == 1 )
        startTime = $1
}
END {
    printf("Period: %.2f minutes\n", ($1 - startTime) / 60)
    printf("Start time: %s\n", strftime("%d %m %Y", startTime, 1))    # UTC
}
