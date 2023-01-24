#include "main.h"

int main(int argc, char *argv[]){
  int num_bytes = 0x10000;
  reset_cpu();
  FILE *binary = fopen(argv[1], "rb");
  size_t bytes_read = fread(&memory, 1, num_bytes, binary);

  // get the start address of the memory block
  uint16_t start_address = (memory[(bytes_read-4)] << 8) | memory[bytes_read-3];
  pc = start_address;

  if(bytes_read < 0xFFFF)
    move_memory(bytes_read);

  printf("\nHexdump:\n");
  hexdump();
  printf("\n\nPage 0xCO Dump:\n");
  hexdump_mempage(0xC0);
  
  return 0;
}


void move_memory(size_t num_bytes){
  for(uint16_t i=0; i<num_bytes; i++)
    memory[0xFFFF-i] = memory[num_bytes-i-1];
  for(uint16_t i=0; i<(0xFFFF-num_bytes); i++)
    memory[i] = 0;
  return;
}

void hexdump(){
  // TODO: Make this interactive
  for (int i=0; i<=0xFF; i++){
        if ((i < 12) || (i > 0xFD)){
          hexdump_mempage(i);
        } else if (i==12){
            printf("\n*");
        }
    }

  return;
}

void hexdump_mempage(uint8_t page){
  uint16_t start = page << 8;
  for(int i=0; i<0x100; i++){
    if (i%16 == 0){
      printf("\n%04x\t", start+i);
    }
    printf("%02x ", memory[start+i]);
  }

  return;
}