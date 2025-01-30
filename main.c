#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

static int mpg_running = 1;

void handle_exitsig(int sig) {
	mpg_running = 0;
}

void handle_sigcont(int sig) {
	const char* msg = "\e[?25l";
	write(STDOUT_FILENO, msg, 7);
}

int main() {
	if(!isatty(STDOUT_FILENO) || !isatty(STDIN_FILENO)) {
		perror("Textris cannot run without acces to the terminal\n");
		exit(2);
	}
	struct termios setOld, setNew;

	signal(SIGINT, handle_exitsig);
	signal(SIGTSTP, handle_exitsig);
	signal(SIGTERM, handle_exitsig);
	signal(SIGHUP, handle_exitsig);
	signal(SIGCONT, handle_sigcont);

	int res = tcgetattr(STDIN_FILENO, &setOld);
	setNew = setOld;
	setNew.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &setNew);
	printf("\e[?25l");
	fflush(stdout);

	while(mpg_running) {

	}

	tcsetattr(STDIN_FILENO, TCSANOW, &setOld);
	printf("\e[?25h");
	fflush(stdout);
	return 0;
}
