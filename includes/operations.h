#include <stdint.h>
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

// Miscellaneous Instructions (to be added)