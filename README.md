# 6502Emu
6502 Emulator project

Written in GNU C on a Debian based Linux platform with no fancy library requirements currently
I have included a makefile that allows gcc to make the whole project.
The code is written for my own amusement, feel free to clone, change and generally work with it;
all I ask is that you credit me somewhere as the root source.

The intention of the project is simply that I wanted to write a simple cli based 6502/6510 emulator in C,
mostly to show my children how a CPU works at Hex/binary level.  I will look at putting a Youtube series together
for this project at some point, to outline the learning and implementation processes I used in writing the programs.

Each folder first, second and so on is an incremental step.  The comments in the files show what is new, so therefore
comments are not accumulative, in that the comments held in the first files, are not necessarily in the second set and so
on.  This is to try and highlight what steps I was up to (for my own memory), and what thought processes I was going through
at that moment in time.

My overall goals were:

1. Keep the code easy to read
2. Use only necessary data structures that don't make understanding the code too difficult
3. Emulate the legal OP Codes, worry about the illegal ones later, hex only.
4. Don't worry about instruction timings to begin with.
5. try and keep things incremental by creating folders to indicate thought processes and progress.
6. Comment where necessary.
7. Line Assembler and Disassembler are now written and functional
8. NCurses is being used to create a windowed terminal and a pseudo LCD screen for output.
9. There is a keyboard handler written, further work needs doing on this.

So, I have now started to write 6502 assembly routines to do stuff.  ADD and Subtract 32bit numbers.

I am going to do some other routines for various stuff, that way I can see what is needed and missing from the emulator.

This is what I am on with now.
