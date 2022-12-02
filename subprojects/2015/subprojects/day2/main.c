#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

unsigned int calc_paper(char *pos)
{
	char *tok;
	unsigned long int w, h, l;
	unsigned int side1, side2, side3;
	unsigned int paper;

	tok = strsep(&pos, "x");
	w = strtoul(tok, NULL, 10);
	tok = strsep(&pos, "x");
	h = strtoul(tok, NULL, 10);
	tok = strsep(&pos, "\n");
	l = strtoul(tok, NULL, 10);

	side1 = l * w;
	side2 = w * h;
	side3 = h * l;

	paper = 2*side1 + 2*side2 + 2*side3 + MIN(side1, MIN(side2, side3));

	printf("%ldx%ldx%ld = %d\n", l, w, h, paper);

	return paper;
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;
	unsigned int paper = 0;

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

		paper += calc_paper(pos);
	} while(1);

	fclose(f);

	printf("Total Order: %d\n", paper);

	return 0;
}
