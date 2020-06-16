#!/usr/bin/env awk -f

{
    if ($5 == "C3")
        requests[$2]++

    if ($5 == "R3")
        replies[$3]++
}
END {
    print length(requests), "clients"
    for (c in requests) {
        print "Client", c, "requests", requests[c], "replies", replies[c]
    }
}
