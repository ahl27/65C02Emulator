# 65c02 Emulator

This is a C program to emulate a 65c02 processor.

Not really intended for external use, I'm just writing it because it seemed like a fun exercise.

When I get it working, I'll add in instructions to compile and run it.

Priorities for this project are:
  1. Working emulation of 6502 instruction set
  2. (close to) Faithful recreation of clock speeds per instruction
  3. Working emulation of 65c02 instruction set
  4. Faithful recreation of 65c02 bugs (ex. indirect jump to page boundary)

I am currently at (1)--all instructions are implemented. 
Next steps are implementing a way to load hex files as ram, then testing all the operations.
