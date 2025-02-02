#include "render.h"
#include "terminal.h"
#include <string.h>

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

void draw_border() {
	uint32_t midx = (charC - ((20 << scale) + 3)) >> 1;
	uint32_t midy = (lineC - ((20 << scale) + 2)) >> 1;
	const char* frame_color_f = DEFAULT_F;
	const char* frame_color_b = DEFAULT_B;

	for(uint32_t y = 0; y < (21u << scale); ++y) {
		setchar(midx, midy + 1 + y, frame_color_f, frame_color_b, '#');
		setchar(midx + (10u << scale) + 1, midy + 1 + y, frame_color_f, frame_color_b, '#');
		if(y < (11u << scale) + 5) setchar(midx + (10u << scale) * 2 + 2, midy + 1 + y, frame_color_f, frame_color_b, '#');
	}
	char topline[(10u << scale) * 2 + 3 + 1];
	memset(topline, '#', (10u << scale) * 2 + 3);
	topline[(10u << (scale + 1)) + 3] = '\0';
	setstr(midx, midy, frame_color_f, frame_color_b, topline); 
	topline[(10 << scale) + 2] = '\0';
	setstr(midx, midy + (21 << scale), frame_color_f, frame_color_b, topline);
	setstr(midx + (10u << scale) + 1, midy + (21u << scale) / 2 + 1, frame_color_f, frame_color_b, topline);
	setstr(midx + (10u << scale) + 1, midy + (21u << scale) / 2 + 1 + 6, frame_color_f, frame_color_b, topline);
	FLUSHOUT;
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
