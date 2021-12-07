#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	unsigned long depth = 0;
	unsigned long horz = 0;
	char temp[255];
	char *pos;
	FILE *f;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "Failed to open file '%s' (%m)\n", argv[1]);
		return 1;
	}
	
	do {
		pos = fgets(temp, 255 - 1, f);
		if (!pos)
			break;

		if (strncmp(pos, "up", strlen("up")) == 0) {
			depth -= strtoul(&pos[strlen("up") + 1], NULL, 10);
		} else if (strncmp(pos, "down", strlen("down")) == 0) {
			depth += strtoul(&pos[strlen("down") + 1], NULL, 10);
		} else if (strncmp(pos, "forward", strlen("forward")) == 0) {
			horz += strtoul(&pos[strlen("forward") + 1], NULL, 10);
		} else
			fprintf(stderr, "Unknown command: '%s'\n", pos);

		printf("Horizontal %ld Depth %ld\n", horz, depth);

	} while(pos);

	fclose(f);

	printf("Result: %ld\n", horz * depth);

	return 0;
}
