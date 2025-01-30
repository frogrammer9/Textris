#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "terminal.h"

int main() {
	uint32_t charC = 0, lineC = 0;
	if(terminal_setup(&charC, &lineC)) { perror("Failed to initialize terminal"); exit(1); }
	if(charC < 10 + 2 + 10) return 1;
	if(lineC < 20 + 2) return 2;

	char c = EOF;
	while((c = getchar()) != 'q') {
		if(c != EOF) printf("%c", c);
	}

	return 0;
}
