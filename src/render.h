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

typedef enum {
	tet_I, tet_O, tet_T, tet_L, tet_J, tet_S, tet_Z
} tetromino_type;

#define BITMAP_SIZE 200 
//I know those arrays aren't bitmaps but I named them when they were.
int render_init(uint8_t scale, uint16_t charC, uint16_t lineC, uint8_t do_color);
void draw_border();
void bitmap_init(cell bitmap[BITMAP_SIZE]);
void bitmap_set(cell bitmap[BITMAP_SIZE], uint8_t x, uint8_t y, char c, uint8_t color);
void bitmap_shift_down(cell bitmap[BITMAP_SIZE], uint8_t amount);
void bitmap_shift_right(cell bitmap[BITMAP_SIZE]);
void bitmap_shift_left(cell bitmap[BITMAP_SIZE]);
void bitmap_remove_line(cell bitmap[BITMAP_SIZE], uint8_t lineNumber, uint8_t lineAmount);
void bitmap_rotate(cell bitmap[BITMAP_SIZE], uint8_t pivotX, uint8_t pivotY, tetromino_type type); //NOTE: This is very limited, works just for tetrominos
uint8_t will_bitmap_overlap(cell bitmap1[BITMAP_SIZE], cell bitmap2[BITMAP_SIZE]);
void draw_bitmap(cell bitmap[BITMAP_SIZE], cell bitmap_cp[BITMAP_SIZE]);

//////////////////////////////////////
//// Game
//////////////////////////////////////


uint8_t update(); //returns score acquired during the update
tetromino_type get_next();

#endif
