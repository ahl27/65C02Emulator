#ifndef OPERATIONSINCLUDE
#define OPERATIONSINCLUDE

#include <stdint.h>
#include "registers.h"
#include "core_functionality.h"

typedef uint8_t byte;

// Group 1 Instructions
void ORA(byte *addr);

void AND(byte *addr);

void EOR(byte *addr);

void ADC(byte *addr);

void STA(byte *addr);

void LDA(byte *addr);

void CMP(byte *addr);

void SBC(byte *addr);


// Group 2 Instructions
void ASL(byte *addr);

void ROL(byte *addr);

void LSR(byte *addr);

void ROR(byte *addr);

void STX(byte *addr);

void LDX(byte *addr);

void DEC(byte *addr);

void INC(byte *addr);

// Group 3 Instructions
void BIT(byte *addr);

void JMP(byte *addr);

void STY(byte *addr);

void LDY(byte *addr);

void CPY(byte *addr);

void CPX(byte *addr);

// Single-Byte Instructions
// indexes 0x08 - 0xF8

// Going to combine some instructions here
void push_to_stack(byte *registerptr);
void pull_from_stack(byte *registerptr);
void transfer_registers(byte *reg1, byte *reg2);
void set_clear_flag(uint8_t shiftamt, uint8_t val);

void NOP();

// Interrupt / Subroutine Instructions
void BRK();

void JSR();

void RTI();

void RTS();

// 65C02 Instruction Set

void bit_set_clear(byte high);
void test_and_branch(byte high);
void STZ(byte *addr);
void TSB(byte *addr);
void TRB(byte *addr);

#endif