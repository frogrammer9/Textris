#include "render.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

cell bitmap[200]; 

static uint8_t charC = 0, lineC = 0, scale = 0;

//////////////////////////////////////
/// Rendering
//////////////////////////////////////

int render_init(uint8_t s, uint8_t cC, uint8_t lC) {
	for(uint8_t i = 0; i < 200; ++i) {
			bitmap[i].c = ' ';
			bitmap[i].colb = DEFAULT_B;
			bitmap[i].colf = DEFAULT_F;
	}
	charC = cC, lineC = lC,  scale = s;
	return 0;
}

void draw_border(uint8_t do_color) {
	static uint8_t do_gen = 1;
	static char buffer[1200] = {0};
	if(do_gen) {
		char* at = buffer;
		uint32_t midx = (charC - ((20 << scale) + 3)) >> 1;
		uint32_t midy = (lineC - ((20 << scale) + 2)) >> 1;
		if(do_color) setcol_at(WHITE_F, WHITE_B, &at);
		for(uint32_t y = 0; y < (21u << scale); ++y) {
			setchar_at(midx, midy + 1 + y, NULL, NULL, '#', &at);
			setchar_at(midx + (10u << scale) + 1, midy + 1 + y, NULL, NULL, '#', &at);
			if(y < (11u << scale) + 5) setchar_at(midx + (10u << scale) * 2 + 2, midy + 1 + y, NULL, NULL, '#', &at);
		}
		char topline[(10u << scale) * 2 + 3 + 1];
		memset(topline, '#', (10u << scale) * 2 + 3);
		topline[(10u << (scale + 1)) + 3] = '\0';
		setstr_at(midx, midy, NULL, NULL, topline, &at);
		topline[(10 << scale) + 2] = '\0';
		setstr_at(midx, midy + (21 << scale), NULL, NULL, topline, &at);
		setstr_at(midx + (10u << scale) + 1, midy + (21u << scale) / 2 + 1, NULL, NULL, topline, &at);
		setstr_at(midx + (10u << scale) + 1, midy + (21u << scale) / 2 + 1 + 6, NULL, NULL, topline, &at);
		do_gen = 0;
		if(do_color) setchar_at(0, 0, DEFAULT_F, DEFAULT_B, '\0', &at);
		else setpos_at(0, 0, &at);
	}
	write(STDOUT_FILENO, buffer, strlen(buffer));
}

uint8_t bitmap_cmpR(cell bm[], cell bmcp[], uint8_t y) {
	for(uint8_t x = 0; x < 10; ++x) {
		cell b = bm[10 * y + x];
		cell bc = bmcp[10 * y + x];
		if(b.c != bc.c || b.colf != bc.colf || b.colb != bc.colb) return 0;
	}
	return 1;
}

void draw_bitmap(uint8_t do_color) { 
	static char buffer[4000] = {0};
	static cell bitmap_cp[200] = {0}; 
	static uint8_t bitmap_cp_init = 1;
	if(bitmap_cp_init) {
		bitmap_cp_init = 0;
		for(uint8_t i = 0; i < 200; ++i) {
			bitmap_cp[i].c = ' ';
			bitmap_cp[i].colb = DEFAULT_B;
			bitmap_cp[i].colf = DEFAULT_F;
		}
	}
	[[maybe_unused]] static cell bitmap_copy[200]; 
	char* at = buffer;
	uint32_t midx = ((charC - ((20 << scale) + 3)) >> 1) + 1;
	uint32_t midy = ((lineC - ((20 << scale) + 2)) >> 1) + 1 + scale;
	for(uint8_t y = 0; y < 20; ++y) {
		char* lastf = NULL, *lastb = NULL;
		if(scale) {
			setpos_at(midx, midy + y * 2, &at);
			for(int i = 0; i < 2; ++i) {
				if(bitmap_cmpR(bitmap, bitmap_cp, y)) break;
				for(uint8_t x = 0; x < 10; ++x) {
					if(lastf != bitmap[10 * y + x].colf && do_color) {
						memcpy(at, bitmap[10 * y + x].colf, strlen(bitmap[10 * y + x].colf)); at += strlen(bitmap[10 * y + x].colf);
						lastf = bitmap[10 * y + x].colf;
					}
					if(lastb != bitmap[10 * y + x].colb && do_color) {
						memcpy(at, bitmap[10 * y + x].colb, strlen(bitmap[10 * y + x].colb)); at += strlen(bitmap[10 * y + x].colb);
						lastb = bitmap[10 * y + x].colb;
					}
					*at = bitmap[10 * y + x].c; at++; 
					*at = bitmap[10 * y + x].c; at++; 
				}
				setpos_at(midx, midy + y * 2 + 1, &at);
			}
		}
		else {
			setpos_at(midx, midy + y, &at);
			for(uint8_t x = 0; x < 10; ++x) {
				if(lastf != bitmap[10 * y + x].colf && do_color) {
					memcpy(at, bitmap[10 * y + x].colf, strlen(bitmap[10 * y + x].colf)); at += strlen(bitmap[10 * y + x].colf);
					lastf = bitmap[10 * y + x].colf;
				}
				if(lastb != bitmap[10 * y + x].colb && do_color) {
					memcpy(at, bitmap[10 * y + x].colb, strlen(bitmap[10 * y + x].colb)); at += strlen(bitmap[10 * y + x].colb);
					lastb = bitmap[10 * y + x].colb;
				}
				*at = bitmap[10 * y + x].c; at++; 
			}
		}
	}
	if(do_color) setchar_at(0, 0, DEFAULT_F, DEFAULT_B, '\0', &at);
	else setpos_at(0, 0, &at);
	write(STDOUT_FILENO, buffer, strlen(buffer));
}

//////////////////////////////////////
/// Game
//////////////////////////////////////

static tetromino_type current_tetromino;
static tetromino_type next_tetromino;

void add_tetromino(tetromino_type t) {

}

uint8_t update() {

}

void move(int8_t x, int8_t y, int8_t r) {

}

tetromino_type get_next() {

}

