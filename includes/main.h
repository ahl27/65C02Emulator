#ifndef MAININCLUDE
#define MAININCLUDE

// Clock time, in nanoseconds
// default is 1000ns = 1MHz, as defined in core_functionality.h
// Can be overwritten here, maybe I'll change it later on or something
// #define CLOCK_TIME 1000

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "registers.h"
#include "operations.h"
#include "core_functionality.h"

#define WIDTH 80
#define HEIGHT 20

void move_memory(size_t num_bytes);
void memory_explorer();
void print_page_curses(WINDOW *menu_win, uint8_t mempage, char *charbuff);
uint8_t process_charbuff(char *arr, int arrlen, uint8_t mempage, char fill);
void clear_charbuff(char *arr, int arrlen, char fill);
void hexdump_mempage(uint8_t page);

#endif