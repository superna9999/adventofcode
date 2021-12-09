#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_H	2000
#define MAX_W	2000

unsigned int floormap[MAX_H][MAX_W];
unsigned int cur_w, cur_h;

#define LOWER(val, cur, check) val = ((cur) < (check) ? (val) : (0))

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos, *tok;
	FILE *f;
	unsigned int i, j, risk = 0;

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

		tok = strsep(&pos, " \r\n");

		if (!cur_w)
			cur_w = strlen(tok);

		for (i = 0; i < cur_w; ++i)
			floormap[cur_h][i] = tok[i] - '0';

		cur_h++;
	} while(1);

	fclose(f);

	for (i = 0; i < cur_h; ++i) {
		for (j = 0; j < cur_w; ++j) {
			unsigned int lower = 1;
			if (i > 0)
				LOWER(lower, floormap[i][j], floormap[i-1][j]);
			if (j > 0)
				LOWER(lower, floormap[i][j], floormap[i][j-1]);
			if (j < (cur_w - 1))
				LOWER(lower, floormap[i][j], floormap[i][j+1]);
			if (i < (cur_h - 1))
				LOWER(lower, floormap[i][j], floormap[i+1][j]);
			if (lower) {
				printf("*%d*", floormap[i][j]);
				risk += floormap[i][j] + 1;
			} else
				printf(" %d ", floormap[i][j]);
		}
		printf("\n");
	}

	printf("Risk: %d\n", risk);

	return 0;
}
