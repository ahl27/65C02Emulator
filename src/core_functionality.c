#include "core_functionality.h"

double elapsedclock = (double)CLOCK_TIME / 1000;

void reset_cpu(){
  // reset registers
  a = 0;
  x = 0;
  y = 0;
  sp = 0xFF;

  // reset memory
  // for now assuming it's all ram
  for(int i=0; i<0xFFFF; i++){
    memory[i] = 0;
  }
}

// We can do this in code, but I'm aiming to emulate the clock speed
// every R/W of a byte is one clock cycle
// Assuming the underlying cpu is negligible, this should accurately
// emulate the correct speeds within a small margin of error.
byte read_byte(uint16_t address){
  byte mem = memory[pc];
  sleep(elapsedclock);
  return mem;
}

void write_byte(byte* address, byte value){
  sleep(elapsedclock);
  memory[address] = value;
  return;
}

void set_pc(uint16_t value){
  sleep(elapsedclock*2);
  pc = value;
  return;
}

void execute_instruction(){
  /*
   *   Instructions for the 65c02 follow a set pattern:
   *      - always 8 bits, of the form aaabbbcc
   *      - aaa are the upper 3 bits, these with cc determine the instruction
   *      - bbb are bits 2-4, these determine the addressing mode
   */

  uint8_t opcode = read_byte(pc++);
  uint8_t cc  = opcode & 0x03;             // AND with 0000 0011 = 0x03
  uint8_t bbb = (opcode & 0x1C) >> 2;      // AND with 0001 1100 = 0x1C
  uint8_t aaa = (opcode & 0xE0) >> 5;      // AND with 1110 0000 = 0xE0
  uint16_t addressl;
  switch(cc){
    case 1:
      address = decode_addrmode_group1(bbb);
      run_instruction_group1(address, aaa);
      break;
    case 2:
      value = decode_addrmode_group23(bbb);
      run_instruction_group2(address, aaa);
      break;
    case 0:
      value = decode_addrmode_group23(bbb);
      run_instruction_group3(address, aaa);
      break;

    // Note that there is no case 3
  }
}

/*
  Helper function to get the address corresponding to the addressing mode
  Later functions will actually read/write to this line
  These are for group 1 (cc == 01 opcodes) instructions
*/
byte* decode_addrmode_group1(byte addrmode){
  uint16_t address;
  switch (addrmode){
    case 0:        // (zero page, X)                  {X, indirect}
      address = read_byte(pc++) + x;
      address &= 0xFF; // addition is without carry
      address = (read_byte(address+1) << 8) | read_byte(address);
      break;
    
    case 1:        // zero page
      address = read_byte(pc++);
      break;
    
    case 2:        // #immediate
      address = pc++;
      break;
    
    case 3:        // absolute
      // TODO: Are these stored little endian? I think so but unsure
      address = (read_byte(pc+1) << 8) | read_byte(pc);
      pc += 2;
      break;
    
    case 4:        // (zero page),Y                   {indirect, y}
      address = read_byte(pc++);
      address = (read_byte(address+1) << 8) | read_byte(address);
      address += y;
      break;
    
    case 5:        // zero page, X
      address = read_byte(pc++) + x;
      break;
    
    case 6:        // absolute, Y
      address = (read_byte(pc+1) << 8) | read_byte(pc);
      address += y;
      pc += 2;
      break;
    
    case 7:        // absolute, X
      address = (read_byte(pc+1) << 8) | read_byte(pc);
      address += x;
      pc += 2;
      break;
  }

  return address+memory;
}

byte* decode_addrmode_group23(byte addrmode){
  uint16_t address;
  switch(addrmode){
    case 0:       // #immediate
      address = pc++;
      break; 
    
    case 1:       // zero page
      address = read_byte(pc++);
      break;

    case 2:       // accumulator
      return (&a);

    case 3:       // absolute
      address = (read_byte(pc+1) << 8) | read_byte(pc);
      pc += 2;
      break;

    // No case 4 exists
    
    case 5:       // zero page, X
      address = read_byte(pc++) + x;
      break;

    // No case 6 exists

    case 7:       // absolute, X
      address = (read_byte(pc+1) << 8) | read_byte(pc);
      address += x;
      break
  }

  return (memory+address);
}

void run_instruction_group1(byte *address, uint8_t highbits){
  switch(highbits){
    case 0:
      ORA(address);
      break;
    case 1:
      AND(address);
      break;
    case 2:
      EOR(address);
      break;
    case 3:
      ADC(address);
      break;
    case 4:
      STA(address);
      break;
    case 5:
      LDA(address);
      break;
    case 6:
      CMP(address);
      break;
    case 7:
      SBC(address);
      break;
  }

  return;
}

void run_instruction_group2(byte *address, uint8_t highbits){
  switch(highbits){
    case 0:
      ASL(address);
      break;
    case 1:
      ROL(address);
      break;
    case 2:
      LSR(address);
      break;
    case 3:
      ROR(address);
      break;
    case 4:
      STX(address);
      break;
    case 5:
      LDX(address);
      break;
    case 6:
      DEC(address);
      break;
    case 7:
      INC(address);
      break;
  }
  return;
}


void run_instruction_group3(byte *address, uint8_t highbits){
  switch(highbits){
    // case 0:
    //    No case 0
    case 1:
      BIT(address);
      break;
    case 2:
    case 3:
      JMP(address);
      break;
    case 4:
      STY(address);
      break;
    case 5:
      LDY(address);
      break;
    case 6:
      CPY(address);
      break;
    case 7:
      CPX(address);
      break;
  }

  return;
}