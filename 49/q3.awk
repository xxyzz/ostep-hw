#!/usr/bin/env awk -f

{
    if ($5 == "C3" && $8 == "getattr")
        users[$2]++

    # size   ($21): size of the file in bytes
    # fileid ($31): number which uniquely identifies the file within its
    #               file system (on UNIX this would be the inumber)

    if ($5 == "R3" && $8 == "getattr")
        fileSize[$31] = $21
}
END {
    for (f in fileSize)
        sum = sum + strtonum("0x"fileSize[f])

    for (user in users)
        print "Client", user, "requests", users[user]

    print length(users), "clients"

    printf("Average file size: %d bytes\n", sum / length(fileSize))
}
