#include <stdint.h>
typedef uint8_t byte;

// Accumulator, X, Y registers
byte a, x, y;

// Program Counter
uint16_t pc;

// Stack Pointer
byte sp;

// Data bus
byte data;

// Address bus
uint16_t address;

// Clock
unsigned int mHz;

// Memory
byte memory[0xFFFF];