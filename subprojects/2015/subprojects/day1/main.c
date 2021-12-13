#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	char temp[8 * 1024];
	char *pos;
	FILE *f;
	unsigned int i;
	int floor;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "Failed to open file '%s' (%s)\n", argv[1], strerror(errno));
		return 1;
	}

	do {
		pos = fgets(temp, sizeof(temp) - 1, f);
		if (!pos)
			break;

		floor = 0;

		for (i = 0; pos[i] && pos[i] != '\n'; ++i) {
			switch (pos[i]) {
			case '(':
				floor++;
				break;
			case ')':
				floor--;
				break;
			default:
				fprintf(stderr, "Invalid input '%c' at %d\n", pos[i], i);
				break;
			}
		}
		pos[i] = '\0';

		printf("%s floor %d\n", pos, floor);

	} while(1);

	fclose(f);

	return 0;
}
