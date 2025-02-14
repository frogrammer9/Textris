#ifndef GAME_H
#define GAME_H

#include <stdint.h>

//////////////////////////////////////
//// Rendering
//////////////////////////////////////

typedef struct {
	char c;
	char* colf;
	char* colb;
} cell;

extern cell bitmap[200]; 

int render_init(uint8_t scale, uint8_t charC, uint8_t lineC, uint8_t do_color);
void draw_border();
void draw_bitmap();

//////////////////////////////////////
//// Game
//////////////////////////////////////

typedef enum {
	tet_I, tet_O, tet_T, tet_L, tet_J, tet_S, tet_Z
} tetromino_type;

uint8_t update(); //returns score acquired during the update
tetromino_type get_next();

//////////////////////////////////////
//// Private
//////////////////////////////////////


#endif
