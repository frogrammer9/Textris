#include "render.h"
#include "terminal.h"
#include <string.h>

cell bitmap[20][10]; 

[[maybe_unused]] static char color_f[17][7] = {
	DEFAULT_F, BLACK_F, DARK_RED_F,
    DARK_GREEN_F, DARK_YELLOW_F, DARK_BLUE_F,
    DARK_MAGENTA_F, DARK_CYAN_F, LIGHT_GRAY_F,
    DARK_GRAY_F, RED_F, GREEN_F,
    ORANGE_F, BLUE_F, MAGENTA_F,
    CYAN_F, WHITE_F			
};

[[maybe_unused]] static char color_b[17][7] = {
	DEFAULT_B, BLACK_B,	DARK_RED_B,
	DARK_GREEN_B, DARK_YELLOW_B, DARK_BLUE_B,
	DARK_MAGENTA_B, DARK_CYAN_B, LIGHT_GRAY_B,
	DARK_GRAY_B, RED_B, GREEN_B,
	ORANGE_B, BLUE_B, MAGENTA_B,
	CYAN_B, WHITE_B,
};

static uint8_t charC = 0, lineC = 0, scale = 0;

int render_init(uint8_t s, uint8_t cC, uint8_t lC) {
	for(uint8_t i = 0; i < 20; ++i) {
		memset(bitmap[i], 0, 10 * sizeof(cell));
	}
	for(uint8_t y = 0; y < 20; ++y) {
		for(uint8_t x = 0; x < 10; ++x) {
			bitmap[y][x].c = '2';
		}
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
	uint32_t midy = ((lineC - ((20 << scale) + 2)) >> 1) + 1;
	for(uint8_t y = 0; y < 20; ++y) {
		for(uint8_t x = 0; x < 10; ++x) {
			//setchar(midx + x, midy + y, color_f[bitmap[y][x].colf], color_b[bitmap[y][x].colb], bitmap[y][x].c);
			setchar(midx + x, midy + y, DEFAULT_F, DEFAULT_B, bitmap[y][x].c);
		}
	}
}
