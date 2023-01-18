#include <stdio.h>
#include "operations.h"
#include "registers.h"

// this is in ms, so 1 = 1MHz
int CLOCK_TIME = 1

// resets registers and memory
void reset_cpu();

// R/W bytes, this will emulate the correct amount of clock cycles
byte read_byte(uint16_t address);
void write_byte(byte* address, byte value);
void set_pc(uint16_t value);

// Runner function, decodes opcode and calls appropriate function
// with correct addressing mode
void execute_instruction();

// Decode middle bytes to determine address to use for operation
byte* decode_addrmode_group1(byte addrmode);
byte* decode_addrmode_group23(byte addrmode);

// Decode high bytes to determine operation to do
// operations are all in operations.c
void run_instruction_group1(byte *address, uint8_t highbits);
void run_instruction_group2(byte *address, uint8_t highbits);
void run_instruction_group3(byte *address, uint8_t highbits);

