#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

typedef struct {
	char c;
	char* colf;
	char* colb;
} cell;

extern cell bitmap[200]; 

int render_init(uint8_t scale, uint8_t charC, uint8_t lineC);
void draw_border(uint8_t do_color);
void draw_bitmap(uint8_t do_color);

#endif
