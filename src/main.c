#include "main.h"

int main(int argc, char *argv[]){
  int num_bytes = 0x10000;
  reset_cpu();
  FILE *binary = fopen(argv[1], "rb");
  size_t bytes_read = fread(&memory, 1, num_bytes, binary);

  // get the start address of the memory block
  uint16_t start_address = (memory[(bytes_read-3)] << 8) | memory[bytes_read-4];
  pc = start_address;

  if(bytes_read < 0xFFFF)
    move_memory(bytes_read);

  //printf("\nHexdump:\n");
  lastop = 0;
  memory_explorer();
  //printf("\nPage 0xCO Dump:\n");
  //hexdump_mempage(0xC0);
  
  return 0;
}


void move_memory(size_t num_bytes){
  for(uint16_t i=0; i<num_bytes; i++)
    memory[0xFFFF-i] = memory[num_bytes-i-1];
  for(uint16_t i=0; i<(0xFFFF-num_bytes); i++)
    memory[i] = 0;
  return;
}

void memory_explorer(){
  WINDOW *mem_win;
  int BUFF_LENGTH = 64;
  char FILLCHAR = ' ';
  uint8_t startpage = (pc & 0xFF00) >> 8;

  // need two extra spots
  BUFF_LENGTH += 2;
  char charbuff[BUFF_LENGTH];
  for (int i=0; i<BUFF_LENGTH; i++)
    charbuff[i] = FILLCHAR;
  charbuff[BUFF_LENGTH-1] = '\0';
  int ctr = 0;
  int c;
  bool quit = false;
  char quitstring[] = "quit";
  char homestring[] = "home";
  uint8_t curpage = startpage;

  initscr();
  if (has_colors()){
    start_color();
    attron(A_BOLD);
  }
  clear();
  noecho();
  cbreak(); /* Line buffering disabled. pass on everything */
  curs_set(0); // turn off cursor
    
  mem_win = newwin(0, 0, 0, 0);
  keypad(mem_win, TRUE);
  wrefresh(mem_win);
  print_page_curses(mem_win, curpage, charbuff);
  while(!quit){ 
    print_page_curses(mem_win, curpage, charbuff);
    c = wgetch(mem_win);
    switch(c){ 
      case KEY_LEFT:
        curpage--;
        break;
      case KEY_RIGHT:
        curpage++;
        break;
      case KEY_UP:
        curpage += 0x10;
        break;
      case KEY_DOWN:
        curpage -= 0x10;
        break;
      case KEY_BACKSPACE:
      case 127:
      case '\b':
        if (ctr > 0)
          charbuff[--ctr] = FILLCHAR;
        break;
      case 0x0A:
      case KEY_ENTER:
      case 0x0D:
        if (strncmp(quitstring, charbuff, 4)==0 || (charbuff[0] == 'q' && charbuff[1] == ' '))
          quit=true;
        else if (strncmp(homestring, charbuff, 4)==0){
          curpage=startpage;
          clear_charbuff(charbuff, BUFF_LENGTH, FILLCHAR);
          ctr = 0;
        }
        else {
          curpage = process_charbuff(charbuff, BUFF_LENGTH, curpage, FILLCHAR, mem_win);
          ctr = 0;
        }
        break;
      default:
        if (c < 128 && c >= 0x20)
          charbuff[ctr++] = c;
        break;
    }
  } 

  clrtoeol();
  refresh();
  endwin();
  return;
}

void clear_charbuff(char *arr, int arrlen, char fill){
  for (int i=0; i<arrlen; i++)
    arr[i] = fill;
  arr[arrlen-1] = '\0';
  return;
}

uint8_t process_charbuff(char *arr, int arrlen, uint8_t mempage, char fill, WINDOW *win){
  char *ptr;
  uint8_t retval = mempage;
  if(arr[0] == 's'){
    int offset = strncmp(arr, "step", 4) == 0 ? 4 : 1;
    ptr = arr+offset;
    while(*ptr==' ' && ptr) ptr++;
    int num_instructions = (int)strtol(ptr, &ptr, 10);
    if (num_instructions == 0) num_instructions = 1;
    uint8_t tmp = lastop;
    lastop = 1;
    for (int i=0; i<num_instructions; i++){
      lastop = execute_instruction();
      if (lastop == 0) break;
    }
  } else if (arr[0] == 'r'){
    // run until a BRK command
    lastop = 1;
    while (lastop != 0){
      lastop = execute_instruction();
      sleep(CLOCK_TIME);
      printw("\n%2X\r  ", lastop);
      print_page_curses(win, retval, arr);
    }
  } else if (arr[0] == 'h') {
      printw("\n\
    \n************************************************\
    \n* Arrows  : Navigate Memory                    *\
    \n* NN      : jump to page 0xNN                  *\
    \n* home    : jump to program counter page       *\
    \n* (s)tep  : step forward one instruction       *\
    \n* (s)tep n: step forward n instructions        *\
    \n* (r)un   : run instructions until BRK         *\
    \n* (h)elp  : help                               *\
    \n* (q)uit  : quit                               *\
    \n************************************************");
  } else {
    retval = (uint8_t)strtol(arr, &ptr, 16);
    if (ptr == arr) 
      retval = mempage;
  } 

  // This does nothing, but it sometimes breaks with a bus error if 
  // we don't print out lastop prior to returning for some reason
  printw("\n%2X\r  ", lastop);
  clear_charbuff(arr, arrlen, fill);

  return retval;
}

void print_page_curses(WINDOW *menu_win, uint8_t mempage, char *charbuff){
  char charout[17];
  charout[16] = '\0';
  uint8_t curval;
  uint16_t start = mempage << 8;
  mvprintw(0, 0, "Current Memory Page: 0x%02X\n", mempage);
  for(int i=0; i<0x100; i++){
    if (i%16 == 0){
      if(i>0) printw("  |%16s|", charout);
      printw("\n%04x\t", start+i);
    } else if (i%8 == 0){
      printw("  ");
    }
    curval = memory[start+i];

    // Reading into chars like hexdump -C
    if ((curval < 0xA0 && curval > 0x7E) || curval < 0x20)
      charout[i%16] = '.';
    else if (curval > 0x9F)
      charout[i%16] = '?';
    else
      charout[i%16] = curval;
    printw("%02x ", curval);
  }
  printw("  |%16s|", charout);

  printw("\n\nRegisters:\n");
  printw("A: 0x%02X\t\tProgram: 0x%04X\n", a, pc);
  printw("X: 0x%02X\t\t  Stack: 0x01%02X\n", x, stackpointer);
  printw("Y: 0x%02X\t\t", y);
  printw(" LastOp: ");
  printw("0x%02X", lastop&0xFF);
  //printw("PC: 0x%04X\tStack: 0x01%02X\n\n", pc, stackpointer);
  char flagschar[] = {'N', 'V', '-', 'B', 'D', 'I', 'Z', 'C'};

  // Printing flags with colors (if enabled)
  printw("\n\nFlags: ");
  if (has_colors()){
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    for (int i=7; i>=0; i--){
      int pairnum = (((1 << i) & flags) > 0) + 1;
      attron(COLOR_PAIR(pairnum));
      addch(flagschar[7-i]);
      attroff(COLOR_PAIR(pairnum));
      addch(' ');
    }

  } else {
    for (int i=7; i>=0; i--){
      printw("%d ",((1 << i) & flags) > 0);
    }
    printw("\n       N V - B D I Z C");
  }
  printw("\n\nPress left or right arrows to move pages, 'h' for help, quit to exit.\n> %s", charbuff);
  refresh();
  return;
}

void hexdump_mempage(uint8_t page){
  char charout[17];
  charout[16] = '\0';
  uint8_t curval;
  uint16_t start = page << 8;
  for(int i=0; i<0x100; i++){
    if (i%16 == 0){
      if(i>0) printf("  |%16s|", charout);
      printf("\n%04x\t", start+i);
    } else if (i%8 == 0){
      printf("  ");
    }
    curval = memory[start+i];

    // Reading into chars like hexdump -C
    if ((curval < 0xA0 && curval > 0x7E) || curval < 0x20)
      charout[i%16] = '.';
    else if (curval > 0x9F)
      charout[i%16] = '?';
    else
      charout[i%16] = curval;
    printf("%02x ", curval);
  }

  return;
}