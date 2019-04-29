# File System Implementation

## Homework (Simulation)

Use this tool, `vsfs.py`, to study how file system state changes as various operations take place. The file system begins in an empty state, with just a root directory. As the simulation takes place, various operations are performed, thus slowly changing the on-disk state of the file system. See the README for details.

### Questions

1. Run the simulator with some different random seeds (say 17, 18, 19, 20), and see if you can figure out which operations must have taken place between each state change.

2. Now do the same, using different random seeds (say 21, 22, 23, 24), except run with the `-r` flag, thus making you guess the state change while being shown the operation. What can you conclude about the inode and data-block allocation algorithms, in terms of which blocks they prefer to allocate?

    First available block.

3. Now reduce the number of data blocks in the file system, to very low numbers (say two), and run the simulator for a hundred or so requests. What types of files end up in the file system in this highly-constrained layout? What types of operations would fail?

    ```
    $ ./vsfs.py -d 2 -n 100 -p -c -s 21
    ```

    Some empty files and links in the root directory.

    `makedir()` and `write()` fail, but shouldn't. The last data block seems can't be used.

4.  Now do the same, but with inodes. With very few inodes, what types of operations can succeed? Which will usually fail? What is the final state of the file system likely to be?

    ```
    $ ./vsfs.py -i 2 -n 100 -p -c -s 21
    ```

    All operations except `unlink()` will fail. Only the first inode is available.

    Change to three inodes ends up with an empty directory or a small file.
