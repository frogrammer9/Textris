#include "render.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

static char stdout_buffer[BUFF_SIZE];
static char* buf_at = stdout_buffer;
uint8_t last_colf = NO_COLOR_F, last_colb = NO_COLOR_B;

//Global vars
static uint16_t charC = 0, lineC = 0;
static uint8_t scale = 0, do_col = 0, midx = 0, midy = 0;

void flushTTY() {
	write(STDOUT_FILENO, stdout_buffer, (uintptr_t)buf_at - (uintptr_t)stdout_buffer);
	buf_at = stdout_buffer;
	last_colf = NO_COLOR_F; 
	last_colb = NO_COLOR_B;
}

void writeTTC(char* data, size_t size) {
	if(size > BUFF_SIZE) {
		write(STDOUT_FILENO, data, size);
		return;
	}
	if((uintptr_t)buf_at - (uintptr_t)stdout_buffer < size) flushTTY();
	memcpy(buf_at, data, size);
	buf_at += size;
}

void setcharTTY(uint8_t x, uint8_t y, uint8_t color, char c) {
	if((uintptr_t)buf_at - (uintptr_t)stdout_buffer < 32) flushTTY();
	if(last_colf == (color >> 4)) color &= NO_COLOR_F;
	if(last_colb == (color & 0x0F)) color &= NO_COLOR_B;
	setchar_at(x, y, color, c, &buf_at);
}

void setcharTTY_nocol(uint8_t x, uint8_t y, char c) {
	if((uintptr_t)buf_at - (uintptr_t)stdout_buffer < 32) flushTTY();
	setchar_at_nocol(x, y, c, &buf_at);
}

void setcharTTY_nopt(char c) {
	if((uintptr_t)buf_at - (uintptr_t)stdout_buffer < 2) flushTTY();
	*(buf_at++) = c;
}

void setcharTTY_nopos(uint8_t color, char c) {
	if((uintptr_t)buf_at - (uintptr_t)stdout_buffer < 16) flushTTY();
	if(last_colf == (color >> 4)) color &= NO_COLOR_F;
	if(last_colb == (color & 0x0F)) color &= NO_COLOR_B;
	setchar_at_nopos(color, c, &buf_at);
}

void setstrTTY(uint8_t x, uint8_t y, uint8_t color, const char* s) {
	if((uintptr_t)buf_at - (uintptr_t)stdout_buffer < (32 + strlen(s))) flushTTY();
	if(last_colf == (color >> 4)) color &= NO_COLOR_F;
	if(last_colb == (color & 0x0F)) color &= NO_COLOR_B;
	setstr_at(x, y, color, s, &buf_at);
}

void setstrTTY_nocol(uint8_t x, uint8_t y, const char* s) {
	if((uintptr_t)buf_at - (uintptr_t)stdout_buffer < (32 + strlen(s))) flushTTY();
	setstr_at_nocol(x, y, s, &buf_at);
}

void setcolTTY(uint8_t color) {
	if((uintptr_t)buf_at - (uintptr_t)stdout_buffer < 16) flushTTY();
	if(last_colf == (color >> 4)) color &= NO_COLOR_F;
	if(last_colb == (color & 0x0F)) color &= NO_COLOR_B;
	setcol_at(color, &buf_at);
}

void setposTTY(uint8_t x, uint8_t y) {
	if((uintptr_t)buf_at - (uintptr_t)stdout_buffer < 16) flushTTY();
	setpos_at(x, y, &buf_at);
}

//////////////////////////////////////
/// Rendering
//////////////////////////////////////

int render_init(uint8_t s, uint16_t cC, uint16_t lC, uint8_t dc) {
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
		if(do_col) setcolTTY(WHITE_F | WHITE_B);
		for(uint32_t y = 0; y < (21u << scale); ++y) {
			setcharTTY_nocol(midx, midy + 1 + y, '#');
			setcharTTY_nocol(midx + (10u << scale) + 1, midy + 1 + y, '#');
			if(y < (11u << scale) + 5) setcharTTY_nocol(midx + (10u << scale) * 2 + 2, midy + 1 + y, '#');
		}
		char topline[(10u << scale) * 2 + 3 + 1];
		memset(topline, '#', (10u << scale) * 2 + 3);
		topline[(10u << (scale + 1)) + 3] = '\0';
		setstrTTY_nocol(midx, midy, topline);
		topline[(10 << scale) + 2] = '\0';
		setstrTTY_nocol(midx, midy + (21 << scale), topline);
		setstrTTY_nocol(midx + (10u << scale) + 1, midy + (21u << scale) / 2 + 1, topline);
		setstrTTY_nocol(midx + (10u << scale) + 1, midy + (21u << scale) / 2 + 1 + 6, topline);
		do_gen = 0;
		if(do_col) setcolTTY(DEFAULT_F | DEFAULT_B);
	}
	flushTTY();
}

uint8_t bitmap_cmpRow(cell bm[BITMAP_SIZE], cell bmcp[BITMAP_SIZE], uint8_t y) { //return 1 if the same
	for(uint8_t x = 0; x < 10; ++x) {
		cell b = bm[10 * y + x];
		cell bc = bmcp[10 * y + x];
		if(b.c != bc.c || b.color != bc.color) return 0;
	}
	return 1;
}

void bitmap_init(cell bitmap[BITMAP_SIZE]) {
	for(uint8_t i = 0; i < 200; ++i) {
		bitmap[i].c = ' ';
		if(do_col) bitmap[i].color = DEFAULT_F | DEFAULT_B;
		else bitmap[i].color = NO_COLOR_F | NO_COLOR_B;
	}
}

void bitmap_set(cell bitmap[BITMAP_SIZE], uint8_t x, uint8_t y, char c, uint8_t color) {
	bitmap[10 * y + x].c = c;
	bitmap[10 * y  + x].color = color;
}

void bitmap_shift_down(cell bitmap[BITMAP_SIZE], uint8_t amount) {
	memmove(&bitmap[10 * amount], bitmap, (200 - 10 * amount) * sizeof(cell));
	for(uint8_t i = 0; i < 10 * amount; ++i) {
		bitmap[i].c = ' ';
		if(do_col) bitmap[i].color = DEFAULT_F | DEFAULT_B;
		else bitmap[i].color = NO_COLOR_F | NO_COLOR_B;
	}
}

void bitmap_shift_right(cell bitmap[BITMAP_SIZE]) {
	for(uint8_t i = 0; i < 200; i += 10) {
		memmove(&bitmap[i + 1], &bitmap[i], 9 * sizeof(cell));
		bitmap[i].c = ' ';
		if(do_col) bitmap[i].color = DEFAULT_F | DEFAULT_B;
		else bitmap[i].color = NO_COLOR_F | NO_COLOR_B;
	}
}

void bitmap_shift_left(cell bitmap[BITMAP_SIZE]) {
	for(uint8_t i = 0; i < 200; i += 10) {
		memmove(&bitmap[i], &bitmap[i + 1], 9 * sizeof(cell));
		bitmap[i + 9].c = ' ';
		if(do_col) bitmap[i + 9].color = DEFAULT_F | DEFAULT_B;
		else bitmap[i + 9].color = NO_COLOR_F | NO_COLOR_B;
	}
}

void bitmap_remove_line(cell bitmap[BITMAP_SIZE], uint8_t lineNumber, uint8_t lineAmount) {
	memmove(&bitmap[10 * lineAmount], bitmap, lineNumber * 10 * sizeof(cell));
	for(uint8_t i = 0; i < 10 * lineAmount; ++i) {
		bitmap[i].c = ' ';
		if(do_col) bitmap[i].color = DEFAULT_F | DEFAULT_B;
		else bitmap[i].color = NO_COLOR_F | NO_COLOR_B;
	}
}

void bitmap_rotate(cell bitmap[BITMAP_SIZE], uint8_t pivotX, uint8_t pivotY, tetromino_type type) {
	cell pivot = bitmap[10 * pivotY + pivotX];
	cell emptyCell = (do_col) ? (cell){.c = ' ', .color = DEFAULT_F | DEFAULT_B } : (cell){.c = ' ', .color = NO_COLOR_F | NO_COLOR_B};
	switch(type) {
		case tet_I: 
			bitmap[10 * (pivotY - 1) + pivotX] = bitmap[10 * (pivotY + 1) + pivotX] = bitmap[10 * (pivotY + 2) + pivotX] = (bitmap[10 * (pivotY - 1) + pivotX].c != ' ') ? emptyCell : pivot;
			bitmap[10 * pivotY + pivotX - 1] = bitmap[10 * pivotY + pivotX + 1] = bitmap[10 * pivotY + pivotX + 2] = (bitmap[10 * (pivotY - 1) + pivotX].c != ' ') ? pivot : emptyCell;
		return;
		case tet_O: return; //Kasane Teto lol
		case tet_T: 
			if	   (bitmap[10 * (pivotY + 0) + pivotX + 1].c == ' ') { bitmap[10 * (pivotY + 1) + pivotX + 0] = emptyCell; bitmap[10 * (pivotY + 0) + pivotX + 1] = pivot; }
			else if(bitmap[10 * (pivotY + 1) + pivotX + 0].c == ' ') { bitmap[10 * (pivotY + 0) + pivotX - 1] = emptyCell; bitmap[10 * (pivotY + 1) + pivotX + 0] = pivot; }
			else if(bitmap[10 * (pivotY + 0) + pivotX - 1].c == ' ') { bitmap[10 * (pivotY - 1) + pivotX + 0] = emptyCell; bitmap[10 * (pivotY + 0) + pivotX - 1] = pivot; }
			else if(bitmap[10 * (pivotY - 1) + pivotX + 0].c == ' ') { bitmap[10 * (pivotY + 0) + pivotX + 1] = emptyCell; bitmap[10 * (pivotY - 1) + pivotX + 0] = pivot; }
		return;
		case tet_S: {
			cell c = (bitmap[10 * pivotY + pivotX - 1].c == ' ') ? pivot : emptyCell;
			bitmap[10 * (pivotY + 0) + pivotX - 1] = bitmap[10 * (pivotY - 1) + pivotX - 1] = c;
			bitmap[10 * (pivotY + 1) + pivotX - 1] = bitmap[10 * (pivotY + 0) + pivotX + 1] = (c.c == ' ') ? pivot : emptyCell;
		} return;
		case tet_Z: {
			cell c = (bitmap[10 * pivotY + pivotX - 1].c == ' ') ? pivot : emptyCell; 
			bitmap[10 * (pivotY + 0) + pivotX - 1] = bitmap[10 * (pivotY + 1) + pivotX + 1] = c;
			bitmap[10 * (pivotY - 1) + pivotX + 1] = bitmap[10 * (pivotY + 0) + pivotX + 1] = (c.c == ' ') ? pivot : emptyCell;
		} return;
		case tet_J: case tet_L: {
			cell c = (bitmap[10 * pivotY + pivotX + 1].c == ' ') ? pivot : emptyCell;
			bitmap[10 * pivotY + pivotX + 1] = bitmap[10 * pivotY + pivotX - 1] = c;
			bitmap[10 * (pivotY + 1) + pivotX] = bitmap[10 * (pivotY - 1) + pivotX] = (c.c == ' ') ? pivot : emptyCell;
			if(bitmap[10 * (pivotY + 1) + pivotX + 1].c != ' ') { bitmap[10 * (pivotY + 1) + pivotX - 1] = pivot; bitmap[10 * (pivotY + 1) + pivotX + 1] = emptyCell; }
			else if(bitmap[10 * (pivotY + 1) + pivotX - 1].c != ' ') { bitmap[10 * (pivotY - 1) + pivotX - 1] = pivot; bitmap[10 * (pivotY + 1) + pivotX - 1] = emptyCell; }
			else if(bitmap[10 * (pivotY - 1) + pivotX - 1].c != ' ') { bitmap[10 * (pivotY - 1) + pivotX + 1] = pivot; bitmap[10 * (pivotY - 1) + pivotX - 1] = emptyCell; }
			else if(bitmap[10 * (pivotY - 1) + pivotX + 1].c != ' ') { bitmap[10 * (pivotY + 1) + pivotX + 1] = pivot; bitmap[10 * (pivotY - 1) + pivotX + 1] = emptyCell; }
		} return;
	} 
}

void draw_bitmap(cell bitmap[BITMAP_SIZE], cell bitmap_cp[BITMAP_SIZE]) { 
	for(uint8_t y = 0; y < 20; ++y) {
		if(bitmap_cmpRow(bitmap, bitmap_cp, y)) continue;
		if(scale) {
			setposTTY(midx, midy + y * 2);
			for(int i = 0; i < 2; ++i) {
				for(uint8_t x = 0; x < 10; ++x) {
					setcharTTY_nopos(bitmap[10 * y + x].color, bitmap[10 * y + x].c);
					setcharTTY_nopos(bitmap[10 * y + x].color, bitmap[10 * y + x].c);
					if(i == 1) {
						bitmap_cp[10 * y + x].c = bitmap[10 * y + x].c;
						bitmap_cp[10 * y + x].color = bitmap[10 * y + x].color;
					}
				}
				if(i == 0) setposTTY(midx, midy + y * 2 + 1);
			}
		}
		else {
			setposTTY(midx, midy + y);
			for(uint8_t x = 0; x < 10; ++x) {
				setcharTTY_nopos(bitmap[10 * y + x].color, bitmap[10 * y + x].c);
				bitmap_cp[10 * y + x].c = bitmap[10 * y + x].c;
				bitmap_cp[10 * y + x].color = bitmap[10 * y + x].color;
			}
		}
	}
	if(do_col) setcolTTY(DEFAULT_F | DEFAULT_B);
	flushTTY();
}
