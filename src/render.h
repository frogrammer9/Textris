#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

typedef enum {
	def, black, dark_red, dark_green, dark_yellow, dark_blue, dark_magenta, dark_cyan, light_gray, dark_gray, red, green, orange, blue, magenta, cyan, white
} colori ;

typedef struct {
	char c;
	colori colf;
	colori colb;
} cell;

extern cell bitmap[20][10]; 

int render_init(uint8_t scale, uint8_t charC, uint8_t lineC);
void draw_border();
void draw_bitmap();

#endif
