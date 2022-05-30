# Paging: Smaller Tables

## Homework (Simulation)

This fun little homework tests if you understand how a multi-level page table works. And yes, there is some debate over the use of the term “fun” in the previous sentence. The program is called, perhaps unsurprisingly: `paging-multilevel-translate.py`; see the README for details.

### Questions

1. With a linear page table, you need a single register to locate the page table, assuming that hardware does the lookup upon a TLB miss. How many registers do you need to locate a two-level page table? A three-level table?

    Just need the top-level page directory base register.

2. Use the simulator to perform translations given random seeds 0, 1, and 2, and check your answers using the `-c` flag. How many memory references are needed to perform each lookup?

    Virtual Address 0x611c: 011000(24) 01000(8) 11100

    page 108: 83(0th) fe e0 da 7f d4 7f eb be 9e d5 ad e4 ac 90 d6 92 d8 c1 f8 9f e1 ed e9 a1(24th) e8 c7 c2 a9 d1 db ff

    0xa1: 1(valid) 0100001(33)

    page  33: 7f(0th) 7f 7f 7f 7f 7f 7f 7f b5(8th) 7f 9d 7f 7f 7f 7f 7f 7f 7f 7f 7f 7f 7f 7f 7f 7f 7f f6 b1 7f 7f 7f 7f

    0xb5: 1(valid) 0110101

    Physical Address: 0110101(53) 11100(28) -> 0x6bc

    page  53: 0f(0th) 0c 18 09 0e 12 1c 0f 08 17 13 07 1c 1e 19 1b 09 16 1b 15 0e 03 0d 12 1c 1d 0e 1a 08(28th) 18 11 00

    Value: 08

3. Given your understanding of how cache memory works, how do you think memory references to the page table will behave in the cache? Will they lead to lots of cache hits (and thus fast accesses?) Or lots of misses (and thus slow accesses)?

    The top-level page directory table fits temporal locality because it will be accessed at every TLB miss and the page entry table are suitable for spatial locality.
