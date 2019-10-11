# 6502Emu
6502 Emulator project

I wanted to write a simple cli based 6502/6510 emulator in C.

My goals were simple:

1. Keep the code easy to read
2. Use only necessary data structures that don't make understanding the code too difficult
3. Emulate the legal OP Codes, worry about the illegal ones later, hex only.
4. Don't worry about instruction timings to begin with.
5. try and keep things incremental by creating folders to indicate thought processes and progress.
6. Comment where necessary.

Those goals are pretty much taken care of in the folders first to fifth.

I then looked at adding other functionality

1. Assembler/disassembler for mnemonics
2. Some form of I/O; screen, serial emulation etc.
3. Code structure for the variables, memory layout etc.

This is what I am on with now.
