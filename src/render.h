#ifndef GAME_H
#define GAME_H

#include <stdint.h>

//////////////////////////////////////
//// Rendering
//////////////////////////////////////

typedef struct {
	char c;
	uint8_t color;
} cell;

#define BITMAP_SIZE 200 
//I know those arrays aren't bitmaps but I named them when they were.
int render_init(uint8_t scale, uint8_t charC, uint8_t lineC, uint8_t do_color);
void draw_border();
void bitmap_init(cell bitmap[BITMAP_SIZE]);
void bitmap_set(cell bitmap[BITMAP_SIZE], uint8_t x, uint8_t y, char c, uint8_t color);
void bitmap_shift_down(cell bitmap[BITMAP_SIZE], uint8_t amount);
void bitmap_shift_right(cell bitmap[BITMAP_SIZE]);
void bitmap_shift_left(cell bitmap[BITMAP_SIZE]);
void bitmap_remove_line(cell bitmap[BITMAP_SIZE], uint8_t lineNumber, uint8_t lineAmount);
uint8_t will_bitmap_overlap(cell bitmap1[BITMAP_SIZE], cell bitmap2[BITMAP_SIZE]);
void draw_bitmap(cell bitmap[BITMAP_SIZE], cell bitmap_cp[BITMAP_SIZE]);

//////////////////////////////////////
//// Game
//////////////////////////////////////

typedef enum {
	tet_I, tet_O, tet_T, tet_L, tet_J, tet_S, tet_Z
} tetromino_type;

uint8_t update(); //returns score acquired during the update
tetromino_type get_next();

#endif
