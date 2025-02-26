#ifndef TERMINAL_H
#define TERMINAL_H
#include <stdint.h>
#include <stdio.h>

#define FLUSHOUT fflush(stdout)

#define NO_COLOR_B		0x0F
#define NO_COLOR_F		0xF0
#define DEFAULT_B		0x00
#define DARK_GREEN_B	0x01
#define DARK_MAGENTA_B	0x02
#define RED_B			0x03
#define ORANGE_B		0x04
#define BLUE_B			0x05
#define CYAN_B			0x06
#define WHITE_B			0x07
#define DEFAULT_F		0x00
#define DARK_GREEN_F	0x10
#define DARK_MAGENTA_F	0x20
#define RED_F			0x30
#define ORANGE_F		0x40
#define BLUE_F			0x50
#define CYAN_F			0x60
#define WHITE_F			0x70

int terminal_setup(uint16_t* charC_out, uint16_t* lineC_out);

void setchar_at(uint8_t x, uint8_t y, uint8_t color, char c, char** at);
void setstr_at(uint8_t x, uint8_t y, uint8_t color, const char* s, char** at);
void setchar_at_nopos(uint8_t color, char c, char** at);
void setchar_at_nocol(uint8_t x, uint8_t y, char c, char** at);
void setstr_at_nopos(uint8_t color, const char* s, char** at);
void setstr_at_nocol(uint8_t x, uint8_t y, const char* s, char** at);
void setpos_at(uint8_t x, uint8_t y, char** at);
void setcol_at(uint8_t color, char** at);

#endif
