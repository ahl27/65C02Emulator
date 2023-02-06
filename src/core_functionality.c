#include "core_functionality.h"

void reset_cpu(){
  // reset registers
  a = 0;
  x = 0;
  y = 0;
  stackpointer = 0xFF;

  // reset memory
  // for now assuming it's all ram
  for(int i=0; i<0xFFFF; i++){
    memory[i] = 0;
  }

  pc = (memory[0xFFFD] << 8) | memory[0xFFFC];
}

// We can do this in code, but I'm aiming to emulate the clock speed
// every R/W of a byte is one clock cycle
// Assuming the underlying cpu is negligible, this should accurately
// emulate the correct speeds within a small margin of error.
byte read_byte(byte *address){
  sleep(CLOCK_TIME);
  return (*address);
}

uint16_t read_address(uint16_t offset){
  uint16_t val = (read_byte(memory+offset+1) << 8);
  val |= read_byte(memory+offset);
  return(val);
}

void write_byte(byte *address, byte value){
  sleep(CLOCK_TIME);
  *address = value;
  return;
}

byte read_pc(){
  byte val = read_byte(memory+pc);
  pc++;
  return(val);
}

void set_pc(uint16_t value){
  sleep(CLOCK_TIME*2);
  pc = value;
  return;
}

uint8_t execute_instruction(){
  /*
   *   Instructions for the 65c02 usually follow a set pattern:
   *      - always 8 bits
   *      - Group 1-3 instructions are of the form aaabbbcc
   *      - aaa are the upper 3 bits, these with cc determine the instruction
   *      - bbb are bits 2-4, these determine the addressing mode
   *
   *      - Single byte instructions have some weirder patterns
   *      - Interrupt expressions are of the form 0aa0 0000 (bbb = 0, cc = 0)
   *      - Bit set/clear, push/pull, increment, transfer ops have aaaa 1000
   *          (bbb=2, cc=0)
   *      - 
   */

  uint8_t opcode = read_pc();
  if(opcode == 0) return 0;

  uint8_t high = opcode >> 4;
  uint8_t low = opcode & 0xF;
  // Check for single byte instructions first since they're easy
  if (low == 8){
    run_instruction_sbyte1(high);
  } else if (low == 0xA){
    run_instruction_sbyte2(high);
  } else {
    uint8_t cc  = opcode & 0x03;             // AND with 0000 0011 = 0x03
    uint8_t bbb = (opcode & 0x1C) >> 2;      // AND with 0001 1100 = 0x1C
    uint8_t aaa = (opcode & 0xE0) >> 5;      // AND with 1110 0000 = 0xE0
    byte *address;
    switch(cc){
      case 1:
        address = decode_addrmode_group1(bbb);
        if (opcode == 0x89){
          // 65C02 expanded set
          BIT(address);
        } else {
          run_instruction_group1(address, aaa);
        }
        break;
      case 2:
        address = decode_addrmode_group23(bbb, aaa);
        if (bbb==4){
          run_instruction_group1(address, aaa);
        } else if (bbb==7 && aaa==4){
          // 65C02 expanded set
          STZ(address);
        } else {
          run_instruction_group2(address, aaa);
        }
        break;
      case 0:
        // check for some 65C02 opcodes here
        if (try65C02opcode(opcode)) break;

        // cc == 0 is a little funky since it also includes branching and interrupts
        if (bbb == 4 || opcode == 0x80){
          // Branching instructions are all aaa 100 00
          // BRA is 0x80
          run_instruction_branching(high);

        } else if (bbb == 0 && !(aaa & 0x4)){
          // interrupt expressions are all 0aa 000 00
          run_instruction_interrupt(aaa);

        } else {
          // If we made it here, it's a standard Group 3 instruction
          // (pass zero because the check for group 2 STX/LDX shouldn't execute)
          address = decode_addrmode_group23(bbb, 0); 
          run_instruction_group3(address, aaa);
        }
        break;

      case 3:
        if (low == 0x7){
          bit_set_clear(high);
        } else if (low == 0xF){
          test_and_branch(high);
        }
        break;
    }
  }
  return opcode;
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
      address = read_pc() + x;
      address &= 0xFF; // addition is without carry
      address = read_address(address);
      break;
    
    case 1:        // zero page
      // TODO: check this
      address = read_pc();
      break;
    
    case 2:        // #immediate
      address = pc++;
      break;
    
    case 3:        // absolute
      // TODO: Are these stored little endian? I think so but unsure
      address = read_address(pc);
      pc += 2;
      break;
    
    case 4:        // (zero page),Y                   {indirect, y}
      address = read_address(read_pc());
      address += y;
      break;
    
    case 5:        // zero page, X
      address = read_pc() + x;
      address &= 0xFF;
      break;
    
    case 6:        // absolute, Y
      address = read_address(pc);
      address += y;
      pc += 2;
      break;
    
    case 7:        // absolute, X
      address = read_address(pc);
      address += x;
      pc += 2;
      break;
  }

  return address+memory;
}

/*
  Helper function to get the address corresponding to the addressing mode
  Later functions will actually read/write to this line
  These are for Group 2/3 (cc == 10,00 opcodes) instructions 
  that aren't single-byte instructions
*/
byte* decode_addrmode_group23(byte addrmode, byte highbits){
  uint16_t address;
  switch(addrmode){
    case 0:       // #immediate
      address = pc++;
      break; 
    
    case 1:       // zero page
      address = read_pc();
      break;

    case 2:       // accumulator
      return (&a);

    case 3:       // absolute
      address = read_address(pc);
      pc += 2;
      break;

    case 4:       // zero page indirect
      address = read_address(read_pc());
      break;
    
    case 5:       // zero page, X
      address = read_pc();
      // need a special check here for LDX/STX
      address += (highbits&6) == 4 ? y : x;
      address &= 0xFF;
      break;

    // No case 6 exists

    case 7:       // absolute, X
      address = read_address(pc);
      address += (highbits&6) == 4 ? y : x;
      break;
  }

  return (memory+address);
}

/*
  Runs an instruction from Group 1
*/
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

/*
  Runs an instruction from Group 2
*/
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

/*
  Runs an instruction from Group 3
*/
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

/*
  Runs a branching instruction
  BRA is currently not supported
*/
void run_instruction_branching(uint8_t highbits){
  // All branching instructions use relative addressing
  // This is a SIGNED 8 bit integer
  int8_t offset = read_pc();
  uint16_t addr = pc + offset;
  uint8_t shift = 0;
  if (highbits == 8){
    set_pc(addr);
    return;
  } 

  byte flag = (highbits & 0xC) >> 2;
  byte value = (highbits>>1) & 1;

  switch(flag){
    case 0:         // negative flag
      shift = 7;
      break;

    case 1:         // overflow flag
      shift = 6;
      break;

    case 2:         // carry flag
      shift = 0;
      break;

    case 3:         // zero flag
      shift = 1;
      break;
  }

  // Branch if flag is equal to value
  if( ((flags & (1 << shift)) > 0) == value ){
    //pc = addr;
    set_pc(addr);
  }

  return;
}


/*
  Runs a single-byte instruction of the form 0xN8
  (N any number)
*/
void run_instruction_sbyte1(uint8_t highbits){
  // This is really verbose
  // Maybe I can come up with a smarter way to do it later
  // This implementation feels intuitive if a bit long
  switch(highbits){
    // Push/Pull operations
    case 0:
      // PHP
      push_to_stack(&flags);
      break;
    case 2:
      // PLP
      pull_from_stack(&flags);
      break;
    case 4:
      // PHA
      push_to_stack(&a);
      break;
    case 6:
      // PLA
      pull_from_stack(&a);
      break;

    // Status Bit set/clear
    case 1:
      // CLC 0001
    case 3:
      // SEC 0011
    case 5:
      // CLI 0101
    case 7:
      // SEI 0111
      set_clear_flag((highbits&0xC)>>1, (highbits&2)>>1);
      break;

    case 11:
      // CLV
      set_clear_flag(6, 0);
      break;

    case 13:
      // CLD
    case 15:
      // SED
      set_clear_flag(3, (highbits&2) >> 1);
      break;

    // Transfer Operations
    case 9:
      // TYA
      transfer_registers(&y, &a);
      break;

    case 10:
      // TAY
      transfer_registers(&a, &y);
      break;

    // Increment / Decrement
    case 8:
      // DEY
      DEC(&y);
      break;

    case 12:
      // INY
      INC(&y);
      break;

    case 14:
      // INX
      INC(&x);
      break;

  }
  return;
}

/*
  Runs a single-byte instruction of the form 0xNA
  (N any number >= 8)
*/
void run_instruction_sbyte2(uint8_t highbits){
  switch(highbits){
    // Highbits will always be at least 8 for 6502
    // 65C02 instructions add lower bit values
    case 1:
      // INC (INA)
      INC(&a);
      break;
    case 3:
      // DEC (DEA)
      DEC(&a);
      break;
    case 5:
      // PHY
      push_to_stack(&y);
      break;
    case 7:
      // PLY
      pull_from_stack(&y);
      break;

    // Transfer Operations
    case 8:
      // TXA
      transfer_registers(&x, &a);
      break;
    case 9:
      // TXS
      transfer_registers(&x, &stackpointer);
      break;
    case 10:
      // TAX
      transfer_registers(&a, &x);
      break;
    case 11:
      transfer_registers(&stackpointer, &x);
      // TSX
      break;

    case 12:
      // DEX
      DEC(&x);
      break;

    case 13:
      // PHX
      push_to_stack(&x);
      break;

    case 15:
      // PLX
      pull_from_stack(&x);
      break;
  }
  return;
}

/*
  Runs one of the interrupt instructions
*/
void run_instruction_interrupt(uint8_t highbits){
  // These will all be of the form 0aa0 0000
  // we're passing in upper three bits (aaa0 0000)
  switch(highbits){
    case 0:  
      BRK();
      break;
    case 1:     
      // This is the JSR Absolute instruction
      JSR();
      break;
    case 2:
      RTI();
      break;
    case 3:
      RTS();
      break;
  }
  return;
}


/*
  Tries to execute one of the expanded 65C02 opcodes
*/
bool try65C02opcode(uint8_t opcode){
  byte *addr;
  uint8_t code = opcode;
  // This approach is a little hacky, but it's the best way I could implement it
  switch(opcode){
    // STZ instructions (0x9E is handled elsewhere)
    case 0x9C:
      // abs
      code = 0x8C; // Convert bbb to 3 for abs indexing
    case 0x64:
      // zp
    case 0x74:
      // zp,X
      addr = decode_addrmode_group1((code & 0x1C) >> 2);
      STZ(addr);
      return true;


    // TRB
    case 0x14:
      // zp
    case 0x1C:
      // abs

    // TSB
    case 0x04:
      // zp
    case 0x0C:
      // abs
      addr = decode_addrmode_group1((code & 0x0C) >> 2);
      (code & 0xF0) > 0 ? TRB(addr) : TSB(addr);
      return true;

    default:
      return false;
  }
}



