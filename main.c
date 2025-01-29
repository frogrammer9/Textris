#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

static int mpg_running = 1;

void handle_sigint(int sig) {
	mpg_running = 0;
}

int main() {
	struct termios setOld, setNew;

	signal(SIGINT, handle_sigint);

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
