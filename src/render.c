#include "render.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

cell bitmap[200]; 
static char stdout_buffer[BUFF_SIZE];
static char* buf_at = stdout_buffer;
static char* last_colf = NULL, *last_colb = NULL;

//Global vars
static uint8_t charC = 0, lineC = 0, scale = 0, do_col = 0, midx = 0, midy = 0;

void flushTTY() {
	write(STDOUT_FILENO, stdout_buffer, (size_t)buf_at - (size_t)stdout_buffer);
	buf_at = stdout_buffer;
	last_colf = last_colb = NULL;
}

void writeTTC(char* data, size_t size) {
	if(size > BUFF_SIZE) {
		write(STDOUT_FILENO, data, size);
		return;
	}
	if((size_t)buf_at - (size_t)stdout_buffer < size) flushTTY();
	memcpy(buf_at, data, size);
	buf_at += size;
}

void setcharTTY(uint8_t x, uint8_t y, const char* fore, const char* back, char c) {
	if((size_t)buf_at - (size_t)stdout_buffer < 32) flushTTY();
	if(last_colf == fore || !do_col) fore = NULL; else last_colf = (char*)fore;
	if(last_colb == back || !do_col) back = NULL; else last_colb = (char*)back;
	setchar_at(x, y, fore, back, c, &buf_at);
}

void setchar_noptTTY(char c) {
	if((size_t)buf_at - (size_t)stdout_buffer < 2) flushTTY();
	*(buf_at++) = c;
}

void setchar_noposTTY(const char* fore, const char* back, char c) {
	if((size_t)buf_at - (size_t)stdout_buffer < 16) flushTTY();
	if(last_colf == fore || !do_col) fore = NULL; else last_colf = (char*)fore;
	if(last_colb == back || !do_col) back = NULL; else last_colb = (char*)back;
	setchar_at_nopos(fore, back, c, &buf_at);
}

void setstrTTY(uint8_t x, uint8_t y, const char* fore, const char* back, const char* s) {
	if((size_t)buf_at - (size_t)stdout_buffer < (32 + strlen(s))) flushTTY();
	if(last_colf == fore || !do_col) fore = NULL; else last_colf = (char*)fore;
	if(last_colb == back || !do_col) back = NULL; else last_colb = (char*)back;
	setstr_at(x, y, fore, back, s, &buf_at);
}

void setcolTTY(const char* fore, const char* back) {
	if((size_t)buf_at - (size_t)stdout_buffer < 16) flushTTY();
	if(last_colf == fore || !do_col) fore = NULL; else last_colf = (char*)fore;
	if(last_colb == back || !do_col) back = NULL; else last_colb = (char*)back;
	setcol_at(fore, back, &buf_at);
}

void setposTTY(uint8_t x, uint8_t y) {
	if((size_t)buf_at - (size_t)stdout_buffer < 16) flushTTY();
	setpos_at(x, y, &buf_at);
}

//////////////////////////////////////
/// Rendering
//////////////////////////////////////

int render_init(uint8_t s, uint8_t cC, uint8_t lC, uint8_t dc) {
	for(uint8_t i = 0; i < 200; ++i) {
		bitmap[i].c = ' ';
		if(dc) {
			bitmap[i].colb = DEFAULT_B;
			bitmap[i].colf = DEFAULT_F;
		}
		else {
			bitmap[i].colb = NULL;
			bitmap[i].colf = NULL;
		}
	}
	charC = cC, lineC = lC,  scale = s, do_col = dc;
	midx = ((charC - ((20 << scale) + 3)) >> 1) + 1;
	midy = ((lineC - ((20 << scale) + 2)) >> 1) + 1 + scale;
	memset(stdout_buffer, 0, BUFF_SIZE);
	return 0;
}

void draw_border() {
	static uint8_t do_gen = 1;
	if(do_gen) {
		uint32_t midx = (charC - ((20 << scale) + 3)) >> 1;
		uint32_t midy = (lineC - ((20 << scale) + 2)) >> 1;
		if(do_col) setcolTTY(WHITE_F, WHITE_B);
		for(uint32_t y = 0; y < (21u << scale); ++y) {
			setcharTTY(midx, midy + 1 + y, NULL, NULL, '#');
			setcharTTY(midx + (10u << scale) + 1, midy + 1 + y, NULL, NULL, '#');
			if(y < (11u << scale) + 5) setcharTTY(midx + (10u << scale) * 2 + 2, midy + 1 + y, NULL, NULL, '#');
		}
		char topline[(10u << scale) * 2 + 3 + 1];
		memset(topline, '#', (10u << scale) * 2 + 3);
		topline[(10u << (scale + 1)) + 3] = '\0';
		setstrTTY(midx, midy, NULL, NULL, topline);
		topline[(10 << scale) + 2] = '\0';
		setstrTTY(midx, midy + (21 << scale), NULL, NULL, topline);
		setstrTTY(midx + (10u << scale) + 1, midy + (21u << scale) / 2 + 1, NULL, NULL, topline);
		setstrTTY(midx + (10u << scale) + 1, midy + (21u << scale) / 2 + 1 + 6, NULL, NULL, topline);
		do_gen = 0;
		if(do_col) setcharTTY(0, 0, DEFAULT_F, DEFAULT_B, '\0');
	}
	flushTTY();
}

uint8_t bitmap_cmpR(cell bm[], cell bmcp[], uint8_t y) {
	for(uint8_t x = 0; x < 10; ++x) {
		cell b = bm[10 * y + x];
		cell bc = bmcp[10 * y + x];
		if(b.c != bc.c || b.colf != bc.colf || b.colb != bc.colb) return 0;
	}
	return 1;
}

void draw_bitmap() { 
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
	for(uint8_t y = 0; y < 20; ++y) {
		if(scale) {
			setposTTY(midx, midy + y * 2);
			for(int i = 0; i < 2; ++i) {
				if(bitmap_cmpR(bitmap, bitmap_cp, y)) break;
				for(uint8_t x = 0; x < 10; ++x) {
					if(do_col) {
						setchar_noposTTY(bitmap[10 * y + x].colf, bitmap[10 * y + x].colb, bitmap[10 * y + x].c);
						setchar_noposTTY(bitmap[10 * y + x].colf, bitmap[10 * y + x].colb, bitmap[10 * y + x].c);
					}
					else {
						setchar_noptTTY(bitmap[10 * y + x].c);
						setchar_noptTTY(bitmap[10 * y + x].c);
					}
				}
				setposTTY(midx, midy + y * 2 + 1);
			}
		}
		else {
			setposTTY(midx, midy + y);
			for(uint8_t x = 0; x < 10; ++x) {
				if(do_col) setchar_noposTTY(bitmap[10 * y + x].colf, bitmap[10 * y + x].colb, bitmap[10 * y + x].c);
				else setchar_noptTTY(bitmap[10 * y + x].c);
			}
		}
	}
	if(do_col) setcharTTY(0, 0, DEFAULT_F, DEFAULT_B, '\0');
	flushTTY();
}

//////////////////////////////////////
/// Game
//////////////////////////////////////

static tetromino_type current_tetromino;
static tetromino_type next_tetromino;

void draw_tet_I(uint8_t x, uint8_t y, uint8_t r);
void draw_tet_O(uint8_t x, uint8_t y, uint8_t r);
void draw_tet_T(uint8_t x, uint8_t y, uint8_t r);
void draw_tet_L(uint8_t x, uint8_t y, uint8_t r);
void draw_tet_J(uint8_t x, uint8_t y, uint8_t r);
void draw_tet_S(uint8_t x, uint8_t y, uint8_t r);
void draw_tet_Z(uint8_t x, uint8_t y, uint8_t r);

//tet_I, tet_O,	tet_T, tet_L, tet_J, tet_S, tet_Z
static const char* tet_colf[] = {CYAN_F, YELLOW_F, DARK_MAGENTA_F, ORANGE_F, BLUE_F, DARK_GREEN_F, RED_F};
static const char* tet_colb[] = {CYAN_B, YELLOW_B, DARK_MAGENTA_B, ORANGE_B, BLUE_B, DARK_GREEN_B, RED_B};
static const char  tet_char[] = {'@', '$', '&', '+', 'o', 'x', '%'};

static void render_tetromino(tetromino_type t, uint8_t x, uint8_t y, uint8_t r) {
	if(x > charC || y > lineC) return;
	r &= 3; //mod 4
	switch(t) {
		case tet_I:	draw_tet_I(x, y, r); break;
		case tet_O:	draw_tet_O(x, y, r); break;
		case tet_T:	draw_tet_T(x, y, r); break;
		case tet_L:	draw_tet_L(x, y, r); break;
		case tet_J:	draw_tet_J(x, y, r); break;
		case tet_S:	draw_tet_S(x, y, r); break;
		case tet_Z:	draw_tet_Z(x, y, r); break;
	}
	flushTTY();
}


uint8_t update() {
	render_tetromino(tet_Z, 4, 4, 0);
	return 0;
}


tetromino_type get_next() {
	return tet_I;
}

void draw_tet_I(uint8_t x, uint8_t y, uint8_t r) {
	if(scale) { x *= 2; y *= 2; }
	x += midx; y += midy;
	if(r == 2) --y;
	else if(r == 3) ++x;
	if((r & 1) == 0) {
		if(scale) {
			setcharTTY(x - 2, y + 0, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); 
			setchar_noptTTY(tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]);
			setchar_noptTTY(tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]);
			setcharTTY(x - 2, y + 1, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); 
			setchar_noptTTY(tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]);
			setchar_noptTTY(tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]);
		}
		else {
			setcharTTY(x, y, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]);
			setchar_noptTTY(tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]);
		}
	}
	else {
		if(scale) {
			setcharTTY(x, y + 0, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); 
			setcharTTY(x, y - 1, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); 
			setcharTTY(x, y - 2, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); 
			setcharTTY(x, y - 3, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); 
			setcharTTY(x, y + 1, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); 
			setcharTTY(x, y + 2, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); 
			setcharTTY(x, y + 3, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); 
			setcharTTY(x, y + 4, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]); setchar_noptTTY(tet_char[tet_I]); 
		}
		else {
			setcharTTY(x, y + 0, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]);
			setcharTTY(x, y + 1, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]);
			setcharTTY(x, y + 2, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]);
			setcharTTY(x, y - 1, tet_colf[tet_I], tet_colb[tet_I], tet_char[tet_I]);
		}
	}
}

void draw_tet_O(uint8_t x, uint8_t y, [[maybe_unused]] uint8_t r) { //center: top left corner
	if(scale) { x *= 2; y *= 2; }
	x += midx; y += midy;
	if(scale) {
		setcharTTY(x, y + 0, tet_colf[tet_O], tet_colb[tet_O], tet_char[tet_O]);
		setchar_noptTTY(tet_char[tet_O]); setchar_noptTTY(tet_char[tet_O]); setchar_noptTTY(tet_char[tet_O]);
		setcharTTY(x, y + 1, tet_colf[tet_O], tet_colb[tet_O], tet_char[tet_O]);
		setchar_noptTTY(tet_char[tet_O]); setchar_noptTTY(tet_char[tet_O]); setchar_noptTTY(tet_char[tet_O]);
		setcharTTY(x, y + 2, tet_colf[tet_O], tet_colb[tet_O], tet_char[tet_O]);
		setchar_noptTTY(tet_char[tet_O]); setchar_noptTTY(tet_char[tet_O]); setchar_noptTTY(tet_char[tet_O]);
		setcharTTY(x, y + 3, tet_colf[tet_O], tet_colb[tet_O], tet_char[tet_O]);
		setchar_noptTTY(tet_char[tet_O]); setchar_noptTTY(tet_char[tet_O]); setchar_noptTTY(tet_char[tet_O]);
	}
	else {
		setcharTTY(x, y + 0, tet_colf[tet_O], tet_colb[tet_O], tet_char[tet_O]);
		setchar_noptTTY(tet_char[tet_O]);
		setcharTTY(x, y + 1, tet_colf[tet_O], tet_colb[tet_O], tet_char[tet_O]);
		setchar_noptTTY(tet_char[tet_O]);
	}
}

void draw_tet_T(uint8_t x, uint8_t y, uint8_t r) {
	if(scale) { x *= 2; y *= 2; }
	x += midx; y += midy;
	switch(r) {
		case 0:
			if(scale) {
				setcharTTY(x - 2, y + 0, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 2, y + 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 2, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 3, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
			}
			else {
				setcharTTY(x - 1, y + 0, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]);

			}
		break;
		case 1:
			if(scale) {
				setcharTTY(x - 0, y - 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y - 2, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 2, y + 0, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 2, y + 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 2, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 3, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
			}
			else {
				setcharTTY(x - 0, y - 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]);
				setcharTTY(x - 1, y + 0, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]);
			}
		break;
		case 2:
			if(scale) {
				setcharTTY(x - 2, y + 0, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 2, y + 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y - 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y - 2, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
			}
			else {
				setcharTTY(x - 1, y + 0, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y - 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]);
			}
		break;
		case 3:
			if(scale) {
				setcharTTY(x - 0, y - 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y - 2, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 0, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setchar_noptTTY(tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 2, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 3, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
			}
			else {
				setcharTTY(x - 0, y - 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]);
				setcharTTY(x - 0, y + 0, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]); setchar_noptTTY(tet_char[tet_T]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_T], tet_colb[tet_T], tet_char[tet_T]);
			}
		break;
	}
}

void draw_tet_L(uint8_t x, uint8_t y, uint8_t r) {
	if(scale) { x *= 2; y *= 2; }
	x += midx; y += midy;
	switch(r) {
		case 0:
			if(scale) {
				setcharTTY(x - 2, y + 0, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 2, y + 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 2, y + 2, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 2, y + 3, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
			}
			else {
				setcharTTY(x - 1, y + 0, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 1, y + 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]);

			}
		break;
		case 1:
			if(scale) {
				setcharTTY(x - 0, y + 2, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 0, y + 3, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 2, y - 0, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 2, y - 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 0, y + 0, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
			}
			else {
				setcharTTY(x - 0, y + 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]);
				setcharTTY(x - 1, y - 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 0, y + 0, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]);
			}
		break;
		case 2:
			if(scale) {
				setcharTTY(x - 2, y + 0, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 2, y + 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x + 2, y - 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x + 2, y - 2, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
			}
			else {
				setcharTTY(x - 1, y + 0, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x + 1, y - 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]);
			}
		break;
		case 3:
			if(scale) {
				setcharTTY(x - 0, y - 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 0, y - 2, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 0, y + 2, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setchar_noptTTY(tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 0, y + 0, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
			}
			else {
				setcharTTY(x - 0, y + 0, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]); setchar_noptTTY(tet_char[tet_L]);
				setcharTTY(x - 0, y - 1, tet_colf[tet_L], tet_colb[tet_L], tet_char[tet_L]);
			}
		break;
	}
}

void draw_tet_J(uint8_t x, uint8_t y, uint8_t r) {
	if(scale) { x *= 2; y *= 2; }
	x += midx; y += midy;
	switch(r) {
		case 0:
			if(scale) {
				setcharTTY(x - 2, y + 0, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 2, y + 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x + 2, y + 2, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x + 2, y + 3, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
			}
			else {
				setcharTTY(x - 1, y + 0, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x + 1, y + 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]);

			}
		break;
		case 1:
			if(scale) {
				setcharTTY(x - 0, y - 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 0, y - 2, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 2, y + 2, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 2, y + 3, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 0, y + 0, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
			}
			else {
				setcharTTY(x - 0, y - 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]);
				setcharTTY(x - 1, y + 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 0, y + 0, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]);
			}
		break;
		case 2:
			if(scale) {
				setcharTTY(x - 2, y + 0, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 2, y + 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 2, y - 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 2, y - 2, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
			}
			else {
				setcharTTY(x - 1, y + 0, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 1, y - 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]);
			}
		break;
		case 3:
			if(scale) {
				setcharTTY(x - 0, y + 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 0, y + 2, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 0, y - 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 0, y - 2, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setchar_noptTTY(tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 0, y + 0, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
			}
			else {
				setcharTTY(x - 0, y + 0, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]);
				setcharTTY(x - 0, y - 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]); setchar_noptTTY(tet_char[tet_J]);
				setcharTTY(x - 0, y + 1, tet_colf[tet_J], tet_colb[tet_J], tet_char[tet_J]);
			}
		break;
	}
}

void draw_tet_S(uint8_t x, uint8_t y, uint8_t r) {
	if(r == 2) --y;
	else if(r == 3) ++x;
	if(scale) { x *= 2; y *= 2; }
	x += midx; y += midy;
	if((r & 1) == 0) {
		if(scale) {
			setcharTTY(x - 0, y, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setchar_noptTTY(tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setcharTTY(x - 0, y + 1, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setchar_noptTTY(tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setcharTTY(x - 2, y + 2, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setchar_noptTTY(tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setcharTTY(x - 2, y + 3, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setchar_noptTTY(tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
		}
		else {
			setcharTTY(x - 0, y - 0, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setcharTTY(x - 1, y + 1, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
		}
	}
	else {
		if(scale) {
			setcharTTY(x - 2, y, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setchar_noptTTY(tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setcharTTY(x - 2, y + 1, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setchar_noptTTY(tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setcharTTY(x - 2, y - 1, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setcharTTY(x - 2, y - 2, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setcharTTY(x - 0, y + 2, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setcharTTY(x - 0, y + 3, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]); 
		}
		else {
			setcharTTY(x - 1, y - 0, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]); setchar_noptTTY(tet_char[tet_S]);
			setcharTTY(x - 1, y - 1, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]);
			setcharTTY(x - 0, y + 1, tet_colf[tet_S], tet_colb[tet_S], tet_char[tet_S]);
		}
	}
}

void draw_tet_Z(uint8_t x, uint8_t y, uint8_t r) {
	if(r == 2) --y;
	else if(r == 3) ++x;
	if(scale) { x *= 2; y *= 2; }
	x += midx; y += midy;
	if((r & 1) == 0) {
		if(scale) {
			setcharTTY(x - 2, y, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setchar_noptTTY(tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setcharTTY(x - 2, y + 1, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setchar_noptTTY(tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setcharTTY(x - 0, y + 2, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setchar_noptTTY(tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setcharTTY(x - 0, y + 3, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setchar_noptTTY(tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
		}
		else {
			setcharTTY(x - 1, y - 0, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setcharTTY(x - 0, y + 1, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
		}
	}
	else {
		if(scale) {
			setcharTTY(x - 2, y, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setchar_noptTTY(tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setcharTTY(x - 2, y + 1, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setchar_noptTTY(tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setcharTTY(x - 0, y - 1, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setcharTTY(x - 0, y - 2, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setcharTTY(x - 2, y + 2, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setcharTTY(x - 2, y + 3, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
		}
		else {
			setcharTTY(x - 1, y - 0, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]); setchar_noptTTY(tet_char[tet_Z]);
			setcharTTY(x - 0, y - 1, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]);
			setcharTTY(x - 1, y + 1, tet_colf[tet_Z], tet_colb[tet_Z], tet_char[tet_Z]);
		}
	}
}
