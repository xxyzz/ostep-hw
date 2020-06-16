#!/usr/bin/env awk -f

{
    if ($5 == "C3")
        startTime[$6] = $1
    if ($5 == "R3") {
        latency = ($1 - startTime[$6]) * 1000
        if (latency <= 1)
            print latency
        #print "ID", $6, "latency", ($1 - startTime[$6]) * 1000, "milliseconds"
        #sum = sum + $1 - startTime[$6]
    }
}
#END {
#    print "Average latency", sum / length(startTime) * 1000, "milliseconds"
#}
