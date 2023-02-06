#ifndef COREHEADERINCLUDE
#define COREHEADERINCLUDE

#include <stdio.h>
#include <ncurses.h>
#include "registers.h"
#include "operations.h"


#ifdef WIN32
  #include <windows.h>
  // sub millisecond timing not supported in windows
  #define sleep(x) Sleep(1);

#elif _POSIX_C_SOURCE >= 199309L
  #include <time.h>
  #define sleep(x) {\
      struct timespec ts;\
      ts.tv_nsec = x;\
      nanosleep(&ts, NULL);\
  }
#else
  #include <unistd.h>
  #define sleep(x) usleep(x * 1000);
#endif

// this is in nanoseconds, so 1000 = 1MHz
#ifndef CLOCK_TIME
  #define CLOCK_TIME 1
#endif

// resets registers and memory
void reset_cpu();

// R/W functions with clock cycle emulation

// R/W bytes at an address
byte read_byte(byte* address);
void write_byte(byte* address, byte value);

// Read address from memory in little-endian format
uint16_t read_address(uint16_t offset);

// read value at program counter and increment or set program counter directly 
byte read_pc();
void set_pc(uint16_t value);

// Runner function, decodes opcode and calls appropriate function
// with correct addressing mode
uint8_t execute_instruction();

// Decode middle bytes to determine address to use for operation
byte* decode_addrmode_group1(byte addrmode);
byte* decode_addrmode_group23(byte addrmode, byte highbits);

// Decode high bytes to determine operation to do
// operations are all in operations.c
void run_instruction_group1(byte *address, uint8_t highbits);
void run_instruction_group2(byte *address, uint8_t highbits);
void run_instruction_group3(byte *address, uint8_t highbits);
void run_instruction_branching(uint8_t highbits);
void run_instruction_sbyte1(uint8_t highbits);
void run_instruction_sbyte2(uint8_t highbits);
void run_instruction_interrupt(uint8_t highbits);

// Additional 65C02 instructions
bool try65C02opcode(uint8_t opcode);

#endif