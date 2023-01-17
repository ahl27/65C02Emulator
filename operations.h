#include <stdint.h>
typedef uint8_t byte;

// Group 1 Instructions
void ORA(uint16_t addr);

void AND(uint16_t addr);

void EOR(uint16_t addr);

void ADC(uint16_t addr);

void STA(uint16_t addr);

void LDA(uint16_t addr);

void CMP(uint16_t addr);

void SBC(uint16_t addr);


// Group 2 Instructions
void ASL(uint16_t addr);

void ROL(uint16_t addr);

void LSR(uint16_t addr);

void ROR(uint16_t addr);

void STX(uint16_t addr);

void LDX(uint16_t addr);

void DEC(uint16_t addr);

void INC(uint16_t addr);

// Group 3 Instructions
void BIT(uint16_t addr);

void JMP(uint16_t addr);

void STY(uint16_t addr);

void LDY(uint16_t addr);

void CPY(uint16_t addr);

void CPX(uint16_t addr);