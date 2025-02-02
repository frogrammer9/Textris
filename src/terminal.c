#include "terminal.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

static struct termios term_old, term_new;

static uint8_t* draw_frame = 0;

static void handle_exitsig([[maybe_unused]] int sig) {
	_exit(0);
}

static void handle_sigcont([[maybe_unused]] int sig) {
	int res = tcsetattr(STDIN_FILENO, TCSANOW, &term_new);
	if(res < 0) { perror("tcsetattr failed"); }
	const char* str = "\e[?25l\e[H\e[J";
	write(STDOUT_FILENO, str, strlen(str)); 
	*draw_frame = 1;
}

static void handle_sigtstp([[maybe_unused]] int sig) {
	int res = tcsetattr(STDIN_FILENO, TCSANOW, &term_old);
	if(res < 0) { perror("tcsetattr failed"); }
	const char* str = "\e[?25h";
	write(STDOUT_FILENO, str, strlen(str)); 
	signal(SIGTSTP, SIG_DFL);
    raise(SIGTSTP);
}

static void terminal_restore() {
	int res = tcsetattr(STDIN_FILENO, TCSANOW, &term_old);
	if(res < 0) { perror("tcsetattr failed"); }
	const char* str = "\e[39m\e[49m\e[H\e[J\e[?25h";
	write(STDOUT_FILENO, str, strlen(str));
}

int terminal_setup(uint8_t* charC_out, uint8_t* lineC_out, uint8_t* draw_frame_flag) {
	signal(SIGINT , handle_exitsig);
	signal(SIGTERM, handle_exitsig);
	signal(SIGHUP , handle_exitsig);
	signal(SIGTSTP, handle_sigtstp);
	signal(SIGCONT, handle_sigcont);

	draw_frame = draw_frame_flag;
	*draw_frame_flag = 1;

	if(!isatty(STDOUT_FILENO) || !isatty(STDIN_FILENO)) {
		perror("Textris cannot run without acces to the terminal");
		exit(2);
	}
	atexit(terminal_restore);
	int res = tcgetattr(STDIN_FILENO, &term_old);
	if(res < 0) return -1;
	term_new = term_old;
	term_new.c_lflag &= ~(ICANON | ECHO);
	res = tcsetattr(STDIN_FILENO, TCSANOW, &term_new);
	if(res < 0) return -1;
	const char* str = "\e[?25l\e[H\e[J";
	write(STDOUT_FILENO, str, strlen(str));

	struct winsize ws;
	res = ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	if(res) return -1;
	*charC_out = ws.ws_col, *lineC_out = ws.ws_row;

	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
	return 0;
}

void setchar(uint8_t x, uint8_t y, const char* fore, const char* back, char c) {
	printf("\e[%d;%dH%s%s%c", y, x, fore, back, c);
}

void setstr(uint8_t x, uint8_t y, const char* fore, const char* back, const char* s) {
	printf("\e[%d;%dH%s%s%s", y, x, fore, back, s);
}

