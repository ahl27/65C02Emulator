# 65c02 Emulator

This is a C program to emulate a 65c02 processor. Should run on OSX and Linux, I have no plans to support Windows (sorry!)

You can follow my blog posts about this project at [on my website](https://www.ahl27.com/tags/#emulator)!

Priorities for this project are:
  1. Working emulation of 6502 instruction set [DONE!]
  2. Working emulation of 65c02 instruction set [DONE!]
  3. Recreating clock cycles per instruction
  4. Faithful recreation of 65c02 bugs (ex. indirect jump to page boundary)

**Note that this is a personal project, if you need really accurate emulation of a 6502 please check out [Symon](https://github.com/sethm/symon)**. At this point I am no longer developing this project unless bug reports are filed; I consider it to be finished to a satisfactory degree. There are some improvements that could be made (listed at the end), but I have other projects that are higher priority for me. I may circle back to this at some point in the future.

## Running the Project

To install, clone the repository and run `make` from within the base directory of the project. 

To run 6502 assembly code, run `build/6502emu.out path/to/file.o65`. As an example, you can run
tests in the test suite such as:

```
> ./build/6502emu.out tests/loadstore.o65
```

This will open the GUI and display the memory and registers. Use the left/right arrow keys to move one page, or use the up/down keys (or scroll) to move 16 pages at a time. You can also type in a specific page to go to (ex. `01` to jump to page `0x10`). Typing `home` will return you to the page the program counter is currently on. `step` runs a single instruction, and `step n` will run `n` instructions. `run` will run the program until a `BRK` command is encountered, at which point control is returned to the user. Running `run` again will then run until the second `BRK` command is encountered, and so on. You can use `help` to display this information in the GUI, and `quit` to quit.

The system treats all memory as RAM; I currently haven't implemented the ability to use ROM or other auxilliary chips. Memory is automatically loaded in to be aligned with the end of RAM, so the last instruction in assembly code will always be at location `0xFFFF`. For example, if you were to pass in a program of size 32KiB, it would be automatically loaded in memory from `0x9000-0xFFFF`. A 16KiB program is loaded in memory to `0xC000-0xFFFF`.

![](examplegui.png)

## Known Issues / Future Updates

1. Flag display will be adjusted at a later date to support people with difficulty differentiating red/green
2. GUI will look like garbage if the window is too small--make sure your window is large enough
3. Programs are loaded directly into RAM and executed, no support yet for ROM or other chips. Planning to add later.
4. Clock cycles are not quite the same as the original 6502, I'll try to get to it later. They're pretty close, though.
5. Interrupts are not supported.
6. This is not tested (nor expected to run) on Windows.
