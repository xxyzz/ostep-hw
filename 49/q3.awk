#! /usr/bin/awk -f

{
    if ($5 == "C3" && $8 == "getattr") {
        fileSize[$10] = $6    # request/reply pair id
        users[$2]++
    } else if ($5 == "R3" && $8 == "getattr") {
        for (f in fileSize) {
            if (fileSize[f] == $6)
                fileSize[f] = $21    # file size
        }
    }
}
END {
    for (f in fileSize)
        sum = sum + strtonum("0x"fileSize[f])
    
    for (user in users)
        print "User", user, "request times", users[user]

    print length(users), "users"

    printf("Average file size: %d\n", sum / length(fileSize))
}
