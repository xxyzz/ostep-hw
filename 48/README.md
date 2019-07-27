# Distributed Systems

## Homework (Code)

In this section, we’ll write some simple communication code to get you familiar with the task of doing so. Have fun!

### Questions

1. Using the code provided in the chapter, build a simple UDP-based server and client. The server should receive messages from the client, and reply with an acknowledgment. In this first attempt, do not add any retransmission or robustness (assume that communication works perfectly). Run this on a single machine for testing; later, run it on two different machines.

    ```
    $ man 3 getaddrinfo
    $ man 7 socket
    ```

2. Turn your code into a **communication library**. Specifically, make your own API, with send and receive calls, as well as other API calls as needed. Rewrite your client and server to use your library instead of raw socket calls.

3. Add reliable communication to your burgeoning communication library,in the form of **timeout/retry**. Specifically, your library should make a copy of any message that it is going to send. When sending it, it should start a timer, so it can track how long it has been since the message was sent. On the receiver, the library should **acknowledge** received messages. The client send should **block** when sending, i.e., it should wait until the message has been acknowledged before returning. It should also be willing to retry sending indefinitely. The maximum message size should be that of the largest single message you can send with UDP. Finally, be sure to perform timeout/retry efficiently by putting the caller to sleep until either an ack arrives or the transmission times out; do not spin and waste the CPU!

    Max UDP payload size: see the [answer](https://github.com/xxyzz/cnata/blob/master/3/wireshark_lab.md#wireshark-lab-udp) of question four.
