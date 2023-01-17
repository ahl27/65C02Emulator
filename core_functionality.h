#include <stdio.h>
#include "operations.h"
#include "registers.h"

// this is in ms, so 1 = 1MHz
int CLOCK_TIME = 1

void reset_cpu();

// R/W bytes, this will emulate the correct amount of clock cycles
byte read_byte(uint16_t address);
void write_byte(uint16_t address, byte value);

void execute_instruction();

byte decode_addrmode_group1(byte addrmode);

uint16_t decode_addrmode_group23(byte addrmode)

