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

// Flags
// N V - B D I Z C
byte flags;

// Address bus
uint16_t address;

// Memory
byte memory[0xFFFF];