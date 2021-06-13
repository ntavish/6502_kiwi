# Kiwi 6502 emulator

This is another 6502 emulator written in C. The goal for this one was learning
how to write one, and hopefully use it to emulate a complete system like the NES
sometime. There are a few pending things and more tests required, which will
be done soon.

## Compiling

It has been tested on linux with gcc, but should work elsewhere.

To compile, just run `make`.

```bash
$ make
gcc -Wall -Wextra -Wno-unused-parameter -O0 -ggdb -I. kiwi.c instructions.c main.c -o kiwi
```

## Running

Once compiled, run as `./kiwi 6502_functional_test.bin` or `make run` to run a functional test of the CPU.
The PC will loop on `0x056D`, which means the supplied tests have passed.

### Notes on `main.c`

The demo program `main.c` connects two things to the 6502 CPU:

1. RAM from `0x0000` to `0x01FF`, i.e. two pages (page is 0xFF bytes), which is
mandatory, and required by the test program.
2. ROM containing the test program from `0x0400` till the `0xFFFF`.

We actually copy contents of the test program from `0x0000` to `0x01ff` to RAM
also.

ROM is a added using `mmap()` and RAM is a `uint8_t` array in the code.

There are no I/O ports or interrupts, and as mentioned above, the tests have
passed when the CPU is looping on PC. The current test binary fails test cases
related to that.
