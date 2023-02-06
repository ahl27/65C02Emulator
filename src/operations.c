#include "operations.h"
//FILE *fp;

/*********
  Group 1 Instructions
*********/
void ORA(byte *addr){
  // OR value with the accumulator
  write_byte(&a, a | (*addr));

  // Set N,Z flags (0x7D = 0111 1101)
  flags = (flags & 0x7D) | 
            ((a & 0x80)) |           // N
            ((a==0) << 1);           // Z
	return;
}

void AND(byte *addr){
  // AND value with the accumulator
  write_byte(&a, a & (*addr));

  // Set N,Z flags (0x7D = 0111 1101)
  flags = (flags & 0x7D) | 
            ((a & 0x80)) |           // N
            ((a==0) << 1);           // Z
	return;
}

void EOR(byte *addr){
  // XOR value with the accumulator
  write_byte(&a, a ^ (*addr));

  // Set N,Z flags (0x7D = 0111 1101)
  flags = (flags & 0x7D) | 
            ((a & 0x80)) |           // N
            ((a==0) << 1);           // Z
	return;
}

void ADC(byte *addr){
  // Add with Carry

  // TODO: some way to interpret decimal mode
  if ((flags & 0x08) > 0){
    // Decimal conversion will go here
  }

  uint16_t res = a + (*addr) + (flags & 1);
  uint8_t truncres = res & 0xFF;
  uint8_t vflag = ((a ^ truncres) & ((*addr) ^ truncres) & 0x80) != 0;
  write_byte(&a, truncres);

  // Set flags N,V,Z,C (0x3C = 0011 1100)
  flags = (flags & 0x3C) | 
              (a & 0x80) |           // N
            (vflag << 6) |           // V
           ((a==0) << 1) |           // Z
           ((res & 0x100) > 0);      // C
	return;
}

void STA(byte *addr){
  // Store value in accumulator to a memory location
  write_byte(addr, a);
	return;
}

void LDA(byte *addr){
  // Load value from memory location to accumulator
  write_byte(&a, *addr);

  // Set N,Z flags (0x7D = 0111 1101)
  flags = (flags & 0x7D) | 
            ((a & 0x80)) |           // N
            ((a==0) << 1);           // Z
	return;
}

void CMP(byte *addr){
  // Compare value against accumulator
  // This doesn't affect the register or the V flag,
  // only flags N,Z,C
  byte vflag = flags & 0x40;
  byte astate = a;
  SBC(addr);

  // Reset accumulator
  a = astate;
  // Reset V Flag
  flags = (flags & 0xBF) | vflag;
	return;
}

void SBC(byte *addr){
  // Subtraction borrrowing from carry
  // Note that the 6502 implements this as just ADC 
  // using the ones complement of the value to subtract
  byte onescomp = ~(*addr);
  ADC(&onescomp);
	return;
}

/*********
  Group 2 Instructions
*********/
void ASL(byte *addr){
  byte newcarry = (*addr) & 0x80;
  byte newval = (*addr) << 1;
  write_byte(addr, newval);

  // Set N,Z,C flags (0x7C = 0111 1100)
  flags = (flags & 0x7D) | 
            ((newval & 0x80))  |     // N
            ((newval==0) << 1) |     // Z
            (newcarry > 0);          // C
	return;
}

void ROL(byte *addr){
	// Rotate Left
  byte newcarry = (*addr) & 0x80;
  byte newval = ((*addr) << 1) | (flags & 1);
  write_byte(addr, newval);

  // Set N,Z,C flags (0x7C = 0111 1100)
  flags = (flags & 0x7D) | 
            ((newval & 0x80))  |     // N
            ((newval==0) << 1) |     // Z
            (newcarry > 0);          // C
  return;
}

void LSR(byte *addr){
	byte newcarry = (*addr) & 1;
  byte newval = (*addr) >> 1;
  write_byte(addr, newval);

  // Set N,Z,C flags (0x7C = 0111 1100)
  flags = (flags & 0x7D) | 
            ((newval & 0x80))  |     // N
            ((newval==0) << 1) |     // Z
            (newcarry > 0);          // C
  return;
}

void ROR(byte *addr){
  byte newcarry = (*addr) & 1;
  byte newval = ((*addr) >> 1) | ((flags & 1) << 7);
  write_byte(addr, newval);

  // Set N,Z,C flags (0x7C = 0111 1100)
  flags = (flags & 0x7D) | 
            ((newval & 0x80))  |     // N
            ((newval==0) << 1) |     // Z
            (newcarry > 0);          // C
  return;
}

void STX(byte *addr){
  // Store value in X to a memory location
  write_byte(addr, x);
  return;
}

void LDX(byte *addr){
  // Load value from memory location to X
  write_byte(&x, *addr);

  // Set N,Z flags (0x7D = 0111 1101)
  flags = (flags & 0x7D) | 
            ((x & 0x80)) |           // N
            ((x==0) << 1);           // Z
  return;
}

// INC and DEC will handle INA, DEA, INX, DEX, INY, DEY
// Just need to pass &a, &x, &y (resp.)
void DEC(byte *addr){
	// Decrement memory address
  byte newval = (*addr)-1;
  write_byte(addr, newval);

  // Set N,Z flags (0x7D = 0111 1101)
  flags = (flags & 0x7D) | 
            ((newval & 0x80)) |           // N
            ((newval==0) << 1);           // Z
  return;
}

void INC(byte *addr){
  // Increment memory address
  byte newval = (*addr)+1;
  write_byte(addr, newval);

  // Set N,Z flags (0x7D = 0111 1101)
  flags = (flags & 0x7D) | 
            ((newval & 0x80)) |           // N
            ((newval==0) << 1);           // Z
	return;
}

/*********
  Group 3 Instructions
*********/

void BIT(byte *addr){
  // BIT sets Z as though the value were AND'd with the accumulator
  // N,V are set to match bits 7,6 (resp.) in the value stored at the address
  // TODO: This function probably executes faster than the 6502 does

  // Sets N,Z,V flags (0x3D = 0011 1101)
  flags = (flags & 0x3D) |
            (((*addr) & a) == 0) << 1 |    // Z
            ((*addr) & 0xC0);              // N,V
	return;
}

void JMP(byte *addr){
  // This won't accurately recreate the indirect jump to last byte of page bug
  // addr is an offset
  // we'll be passed the raw memory location, 
  // but PC should just be the offset from memory
  uint16_t newaddr = addr-memory;
  set_pc(newaddr);
	return;
}

void STY(byte *addr){
  // Store value in Y to a memory location
  write_byte(addr, y);
  return;
}

void LDY(byte *addr){
  // Load value from memory location to Y
  write_byte(&y, *addr);

  // Set N,Z flags (0x7D = 0111 1101)
  flags = (flags & 0x7D) | 
            ((y & 0x80)) |           // N
            ((y==0) << 1);           // Z
  return;
}

void CPY(byte *addr){
  // Compare value against Y register
  // This doesn't affect the register or the V flag,
  // only flags N,Z,C

  byte vflag = flags & 0x40;
  byte astate = a;

  // Little hacky, I'm just temporarily setting a to y
  // so I can reuse SBC code. 
  a = y;
  SBC(addr);

  // Reset accumulator
  a = astate;

  // Reset V Flag
  flags = (flags & 0xBF) | vflag;
  return;
}

void CPX(byte *addr){
	// Compare value against X register
  // This doesn't affect the register or the V flag,
  // only flags N,Z,C

  byte vflag = flags & 0x40;
  byte astate = a;

  // See CPY
  a = x;
  SBC(addr);

  // Reset accumulator
  a = astate;

  // Reset V Flag
  flags = (flags & 0xBF) | vflag;
  return;
}

/*********
  Single-Byte Instructions 
*********/
// Lots of these do the same thing, so we're going to just combine instructions
// instruction decoder can handle picking which to use

// Push from register to stack
void push_to_stack(byte *registerptr){
  uint16_t offset = 0x100 | stackpointer;
  write_byte(memory+offset, *registerptr);
  stackpointer--;
  return;
}

// Pull from stack to register
void pull_from_stack(byte *registerptr){
  stackpointer++;
  uint16_t offset = 0x100 | stackpointer;
  byte val = read_byte(memory+offset);
  // This only updates if we're writing to a register
  // So I'm going to update flags first, 
  flags = (flags & 0x7D) | 
          ((val & 0x80)) |           // N
          ((val==0) << 1);           // Z

  // then pull the value
  // This way if we write to SR it should work as expected
  write_byte(registerptr, val);
  return;
}

// Transfer from register 1 to register 2
void transfer_registers(byte *reg1, byte *reg2){
  byte val = read_byte(reg1);
  write_byte(reg2, val);
  // Modifies N,Z flags unless calling TXS
  if(reg2 != &stackpointer){
    flags = (flags & 0x7D) | 
            ((val & 0x80)) |           // N
            ((val==0) << 1);           // Z
  }
  return;
}

// bit set routines
void set_clear_flag(uint8_t shiftamt, uint8_t val){
  byte newval = val << shiftamt;
  flags = (flags & !(1 << shiftamt)) | newval;
  return;
}

void NOP(){
  return;
}

/*********
  Interrupt/Subroutine Instructions
*********/
void BRK(){
  // Not sure how to implement this one yet
  return;
}

void JSR(){
  // Push status flags, high byte, low byte
  // Note stack grows downward
  uint16_t newloc = read_address(pc);
  set_pc(pc+2);
  byte val_to_push;

  push_to_stack(&flags);
  val_to_push = pc >> 8;

  push_to_stack(&val_to_push);
  val_to_push = pc & 0xFF;

  push_to_stack(&val_to_push);
  JMP(memory+newloc);
  return;
}

void RTI(){
  // Not sure how to implement this one yet
  return;
}

void RTS(){
  // call JMP on location of stack pointer
  // Note stack grows downwards:
  //    FLAGS
  //    high byte
  //    low byte
  // -> (empty)

  // JMP on stackpointer+2 will pull (high << 8) | low
  stackpointer += 1;
  uint16_t addr = read_address(0x100 | stackpointer);
  JMP(memory+addr);
  stackpointer += 1;

  // stackpointer now at correct location, we can pull the status flags
  pull_from_stack(&flags);
  return;
}