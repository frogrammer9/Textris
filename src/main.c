#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <poll.h>
#include <unistd.h>
#include "render.h"
#include "terminal.h"

int main() {
	uint8_t charC = 0, lineC = 0;
	uint8_t draw_frame_flag = 0;
	if(terminal_setup(&charC, &lineC, &draw_frame_flag)) { perror("Failed to initialize terminal"); exit(1); }
	if(charC < 22) { perror("Terminal too narow"); exit(1); }
	if(lineC < 22) { perror("Terminal too short"); exit(1); }

	uint8_t scale = 0;
	if(charC > 42 && lineC > 42)  scale = 1;
	scale = 1;

	render_init(scale, charC, lineC);

	[[maybe_unused]] uint32_t score = 0;

	bitmap[10 * 2 + 1].c = '#';
	bitmap[10 * 2 + 2].c = '#';
	bitmap[10 * 3 + 1].c = '#';
	bitmap[10 * 3 + 2].c = '#';

	char c = EOF;
	clock_t stime ;
	while((c = getchar()) != 'q') {
		stime = clock();
		if(draw_frame_flag) {
			draw_frame_flag = 0;
			draw_border();
		}
		draw_bitmap();



		uint16_t time_to_sleep = (1000 / 20 /*update freq in Hz*/) - (uint32_t)(clock() - stime) * 1000 / CLOCKS_PER_SEC;
		poll(NULL, 0, time_to_sleep);
	}

	return 0;
}
