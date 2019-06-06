#! /usr/bin/awk -f

{
    if ($5 == "C3" && $8 == "getattr") {
        pairID[$6] = $10     # store file handler to pair id array
        users[$2]++
    }
    
    if ($5 == "R3" && $8 == "getattr") {
        fileSize[pairID[$6]] = $21
    }
}
END {
    for (f in fileSize)
        sum = sum + strtonum("0x"fileSize[f])
    
    for (user in users)
        print "Client", user, "requests", users[user]

    print length(users), "clients"

    printf("Average file size: %d\n", sum / length(fileSize))
}
