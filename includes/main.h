#ifndef MAININCLUDE
#define MAININCLUDE

// Clock time, in nanoseconds
// default is 1000ns = 1MHz, as defined in core_functionality.h
// Can be overwritten here, maybe I'll change it later on or something
// #define CLOCK_TIME 1000

#include "registers.h"
#include "operations.h"
#include "core_functionality.h"

void move_memory(size_t num_bytes);
void hexdump();
void hexdump_mempage(uint8_t page);

#endif