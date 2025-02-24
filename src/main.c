#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <poll.h>
#include <unistd.h>
#include "render.h"
#include "terminal.h"

int main(int argc, char** argv) {
	int color = 1;
	int scale = 0;

	struct option opts[] = {
		{"help", no_argument, 0, 'h'},
		{"small", no_argument, &scale, 1},
		{"no-color", no_argument, &color, 0},
		{0, 0, 0, 0}
	};

	int options;
	while((options = getopt_long(argc, argv, "nhs", opts, NULL)) != -1) {
		switch(options) {
			default: { const char* str = "Unknows option\n\n"; write(STDERR_FILENO, str, strlen(str)); }
			case 'h': {
				const char* str = "help (h) - prints this message\nno-color (n) - disables colors\nsmall (s) - renders the game in half the resolution\n";
				write(STDOUT_FILENO, str, strlen(str));
				return 0; break;
			}
			case 's': scale = 1; break;
			case 'n': color = 0; break;
			case 0: break;
		}
	}
	
	uint8_t charC = 0, lineC = 0;
	if(terminal_setup(&charC, &lineC)) { perror("Failed to initialize terminal"); return 1; }
	if(charC < 22) { perror("Terminal too narow"); return 1; }
	if(lineC < 22) { perror("Terminal too short"); return 1; }

	if(scale == 1) scale = 0;
	else { scale = 0; if(charC > 42 && lineC > 42)  scale = 1; }

	render_init(scale, charC, lineC, color);

	cell bitmapS[BITMAP_SIZE], bitmapD[BITMAP_SIZE], bitmapS_cp[BITMAP_SIZE], bitmapD_cp[BITMAP_SIZE];
	bitmap_init(bitmapS);
	bitmap_init(bitmapD);
	bitmap_init(bitmapS_cp);
	bitmap_init(bitmapD_cp);

	bitmap_set(bitmapS, 5, 0, '#', DEFAULT_F | DEFAULT_B);
	bitmap_set(bitmapS, 5, 2, '#', DEFAULT_F | DEFAULT_B);

	[[maybe_unused]] uint32_t score = 0;

	draw_border();
	char c = EOF;
	clock_t stime ;
	while(c != 'q') {
		stime = clock();
		if(read(STDIN_FILENO, &c, 1) <= 0) c = EOF;
		draw_bitmap(bitmapS, bitmapS_cp);

		if(c == 's') bitmap_shift_down(bitmapS, 1);
		if(c == 'a') bitmap_shift_left(bitmapS);
		if(c == 'd') bitmap_shift_right(bitmapS);
		if(c == 't') bitmap_remove_line(bitmapS, 4, 1);

		//update();

		FLUSHOUT;
		uint16_t time_to_sleep = (1000 / 20 /*update freq in Hz*/) - (uint32_t)(clock() - stime) * 1000 / CLOCKS_PER_SEC;
		usleep(time_to_sleep * 1000);
	}

	return 0;
}
