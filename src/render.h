#ifndef GAME_H
#define GAME_H

#include <stdint.h>

//////////////////////////////////////
/// Rendering
//////////////////////////////////////

typedef struct {
	char c;
	char* colf;
	char* colb;
} cell;

extern cell bitmap[200]; 

int render_init(uint8_t scale, uint8_t charC, uint8_t lineC);
void draw_border(uint8_t do_color);
void draw_bitmap(uint8_t do_color);

//////////////////////////////////////
/// Game
//////////////////////////////////////

typedef enum {
	straight, square, T, L, skew
} tetromino_type;

void add_tetromino(tetromino_type t);
uint8_t update(); //returns score acquired during the update
void move(int8_t x, int8_t y, int8_t r);
tetromino_type get_next();

#endif
