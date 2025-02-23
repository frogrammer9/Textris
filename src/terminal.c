#include "terminal.h"
#include "render.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define FCOL_MASK(col) ((col & 0xF0) >> 4)
#define BCOL_MASK(col) (col & 0x0F)
#define NO_COLOR 0x0F

static const char* colorsb[] = {
	"\e[49m", "\e[42m", "\e[45m", "\e[101m",
	"\e[103m", "\e[104m", "\e[106m", "\e[107"
};

static const char* colorsf[] = {
	"\e[39m", "\e[32m", "\e[35m", "\e[91m", 
	"\e[93m", "\e[94m", "\e[96m", "\e[97"
};

static struct termios term_old, term_new;

static void handle_exitsig([[maybe_unused]] int sig) {
	_exit(0); //Very usefull
}

static void handle_sigcont([[maybe_unused]] int sig) {
	int res = tcsetattr(STDIN_FILENO, TCSANOW, &term_new);
	if(res < 0) { perror("tcsetattr failed"); }
	const char* str = "\e[?25l\e[H\e[J";
	write(STDOUT_FILENO, str, strlen(str)); 
	draw_border();
}

static void handle_sigtstp([[maybe_unused]] int sig) {
	int res = tcsetattr(STDIN_FILENO, TCSANOW, &term_old);
	if(res < 0) { perror("tcsetattr failed"); }
	const char* str = "\e[?25l\e[H\e[J";
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

int terminal_setup(uint8_t* charC_out, uint8_t* lineC_out) {
	signal(SIGINT , handle_exitsig);
	signal(SIGTERM, handle_exitsig);
	signal(SIGHUP , handle_exitsig);
	signal(SIGTSTP, handle_sigtstp);
	signal(SIGCONT, handle_sigcont);

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

void setchar_at(uint8_t x, uint8_t y, uint8_t color, char c, char** at) {
	uint8_t fore = FCOL_MASK(color);
	uint8_t back = BCOL_MASK(color);
	if(fore == NO_COLOR && back == NO_COLOR)
		*at += sprintf(*at, "\e[%d;%dH%c", y, x, c);
	else if(fore == NO_COLOR)
		*at += sprintf(*at, "\e[%d;%dH%s%c", y, x, colorsb[back], c);
	else if(back == NO_COLOR)
		*at += sprintf(*at, "\e[%d;%dH%s%c", y, x, colorsf[fore], c);
	else *at += sprintf(*at, "\e[%d;%dH%s%s%c", y, x, colorsf[fore], colorsb[back], c);
}

void setstr_at(uint8_t x, uint8_t y, uint8_t color, const char* s, char** at) {
	const uint8_t fore = FCOL_MASK(color);
	const uint8_t back = BCOL_MASK(color);
	if(fore == NO_COLOR && back == NO_COLOR)
		*at += sprintf(*at, "\e[%d;%dH%s", y, x, s);
	else if(fore == NO_COLOR)
		*at += sprintf(*at, "\e[%d;%dH%s%s", y, x, colorsb[back], s);
	else if(back == NO_COLOR)
		*at += sprintf(*at, "\e[%d;%dH%s%s", y, x, colorsf[fore], s);
	else *at += sprintf(*at, "\e[%d;%dH%s%s%s", y, x, colorsf[fore], colorsb[back], s);
}

void setchar_at_nopos(uint8_t color, char c, char** at) {
	const uint8_t fore = FCOL_MASK(color);
	const uint8_t back = BCOL_MASK(color);
	*at += sprintf(*at, "%s%s%c", colorsf[fore], colorsb[back], c);
}

void setstr_at_nopos(uint8_t color, const char* s, char** at) {
	const uint8_t fore = FCOL_MASK(color);
	const uint8_t back = BCOL_MASK(color);
	*at += sprintf(*at, "%s%s%s", colorsf[fore], colorsb[back], s);
}

void setpos_at(uint8_t x, uint8_t y, char** at) {
	*at += sprintf(*at, "\e[%d;%dH", y, x);
}

void setcol_at(uint8_t color, char** at) {
	const uint8_t fore = FCOL_MASK(color);
	const uint8_t back = BCOL_MASK(color);
	if(fore == NO_COLOR && back == NO_COLOR) return;
	if(fore == NO_COLOR)
		*at += sprintf(*at, "%s", colorsb[back]);
	else if(back == NO_COLOR)
		*at += sprintf(*at, "%s", colorsf[fore]);
	else *at += sprintf(*at, "%s%s", colorsf[fore], colorsb[back]);
}
