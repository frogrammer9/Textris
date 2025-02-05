#include "render.h"
#include "terminal.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

cell bitmap[200]; 
cell bitmap_copy[200]; 

static uint8_t charC = 0, lineC = 0, scale = 0;

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
	static char buffer[3000] = {0};
	if(do_gen) {
		char* at = buffer;
		uint32_t midx = (charC - ((20 << scale) + 3)) >> 1;
		uint32_t midy = (lineC - ((20 << scale) + 2)) >> 1;
		const char* frame_color_f = (do_color) ? WHITE_F : NULL;
		const char* frame_color_b = (do_color) ? WHITE_B : NULL;
		for(uint32_t y = 0; y < (21u << scale); ++y) {
			setchar_at(midx, midy + 1 + y, frame_color_f, frame_color_b, '#', &at);
			setchar_at(midx + (10u << scale) + 1, midy + 1 + y, frame_color_f, frame_color_b, '#', &at);
			if(y < (11u << scale) + 5) setchar_at(midx + (10u << scale) * 2 + 2, midy + 1 + y, frame_color_f, frame_color_b, '#', &at);
		}
		char topline[(10u << scale) * 2 + 3 + 1];
		memset(topline, '#', (10u << scale) * 2 + 3);
		topline[(10u << (scale + 1)) + 3] = '\0';
		setstr_at(midx, midy, frame_color_f, frame_color_b, topline, &at);
		topline[(10 << scale) + 2] = '\0';
		setstr_at(midx, midy + (21 << scale), frame_color_f, frame_color_b, topline, &at);
		setstr_at(midx + (10u << scale) + 1, midy + (21u << scale) / 2 + 1, frame_color_f, frame_color_b, topline, &at);
		setstr_at(midx + (10u << scale) + 1, midy + (21u << scale) / 2 + 1 + 6, frame_color_f, frame_color_b, topline, &at);
		do_gen = 0;
		memcpy(at, DEFAULT_F, strlen(DEFAULT_F));
		at += strlen(DEFAULT_F);
		memcpy(at, DEFAULT_F, strlen(DEFAULT_B));
	}
	write(STDOUT_FILENO, buffer, strlen(buffer));
}

void draw_bitmap() { 
	uint32_t midx = ((charC - ((20 << scale) + 3)) >> 1) + 1;
	uint32_t midy = ((lineC - ((20 << scale) + 2)) >> 1) + 1 + scale;
	for(uint8_t y = 0; y < 20; ++y) {
		for(uint8_t x = 0; x < 10; ++x) {
			if(bitmap[10 * y + x].c != bitmap_copy[10 * y + x].c ||
				bitmap[10 * y + x].colb != bitmap_copy[10 * y + x].colb ||
				bitmap[10 * y + x].colf != bitmap_copy[10 * y + x].colf) {
				if(scale) {
					setchar(midx + x * 2 + 1, midy + y * 2		, bitmap[10 * y + x].colf, bitmap[10 * y + x].colb, bitmap[10 * y + x].c);
					setchar(midx + x * 2	, midy + y * 2 + 1	, bitmap[10 * y + x].colf, bitmap[10 * y + x].colb, bitmap[10 * y + x].c);
					setchar(midx + x * 2 + 1, midy + y * 2 + 1	, bitmap[10 * y + x].colf, bitmap[10 * y + x].colb, bitmap[10 * y + x].c);
					setchar(midx + x * 2	, midy + y * 2		, bitmap[10 * y + x].colf, bitmap[10 * y + x].colb, bitmap[10 * y + x].c);
				}
				else setchar(midx + x, midy + y, bitmap[10 * y + x].colf, bitmap[10 * y + x].colb, bitmap[10 * y + x].c);
				bitmap_copy[10 * y + x].c = bitmap[10 * y + x].c;
				bitmap_copy[10 * y + x].colb = bitmap[10 * y + x].colb;
				bitmap_copy[10 * y + x].colf = bitmap[10 * y + x].colf;
			}
		}
	}
}
