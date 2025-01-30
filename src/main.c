#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "terminal.h"

int main() {
	uint32_t charC = 0, lineC = 0;
	if(terminal_setup(&charC, &lineC)) { perror("Failed to initialize terminal"); exit(1); }
	if(charC < 22) return 1;
	if(lineC < 22) return 2;

	uint32_t scale = 1;
	if(charC > 42 && lineC > 42)  scale = 2;
	scale = 2;

	uint32_t midx = (charC - (20 * scale + 3)) >> 1;
	uint32_t midy = (lineC - (20 * scale + 2)) >> 1;
	const char* frame_color_f = DEFAULT_F;
	const char* frame_color_b = DEFAULT_B;

	for(uint32_t y = 0; y < 21 * scale; ++y) {
		setchar(midx, midy + 1 + y, frame_color_f, frame_color_b, '#');
		setchar(midx + 10 * scale + 1, midy + 1 + y, frame_color_f, frame_color_b, '#');
		if(y < 11 * scale + 5) setchar(midx + (10 * scale) * 2 + 2, midy + 1 + y, frame_color_f, frame_color_b, '#');
	}
	char topline[10 * scale * 2 + 3 + 1];
	memset(topline, '#', 10 * scale * 2 + 3);
	topline[10 * scale * 2 + 3] = '\0';
	setstr(midx, midy, frame_color_f, frame_color_b, topline); 
	topline[10 * scale + 2] = '\0';
	setstr(midx, midy + 21 * scale, frame_color_f, frame_color_b, topline);
	setstr(midx + 10 * scale + 1, midy + (21 * scale) / 2 + 1, frame_color_f, frame_color_b, topline);
	setstr(midx + 10 * scale + 1, midy + (21 * scale) / 2 + 1 + 6, frame_color_f, frame_color_b, topline);
	FLUSHOUT;

	[[maybe_unused]] uint32_t score = 0;

	char c = EOF;
	while((c = getchar()) != 'q') {

	}

	return 0;
}
