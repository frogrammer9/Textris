#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "render.h"
#include "terminal.h"

int main() {
	uint8_t charC = 0, lineC = 0;
	uint8_t draw_frame_flag = 0;
	if(terminal_setup(&charC, &lineC, &draw_frame_flag)) { perror("Failed to initialize terminal"); exit(1); }
	if(charC < 22) { perror("Terminal too narow"); exit(1); }
	if(lineC < 22) { perror("Terminal too short"); exit(1); }

	uint32_t scale = 0;
	if(charC > 42 && lineC > 42)  scale = 1;

	render_init(scale, charC, lineC);

	[[maybe_unused]] uint32_t score = 0;

	bitmap[2][1].c = '#';
	bitmap[2][2].c = '#';
	bitmap[3][1].c = '#';
	bitmap[3][2].c = '#';

	char c = EOF;
	while((c = getchar()) != 'q') {
		if(draw_frame_flag) {
			draw_frame_flag = 0;
			draw_border();
		}
		draw_bitmap();
	}

	return 0;
}
