#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_H	2000
#define MAX_W	2000
#define MAX_SIZES 3

unsigned int radar[MAX_H][MAX_W];
unsigned int floormap[MAX_H][MAX_W];
unsigned int sizes[MAX_SIZES];
unsigned int cur_w, cur_h;

#define LOWER(val, cur, check) val = ((cur) < (check) ? (val) : (0))

unsigned int explore(unsigned int x, unsigned int y)
{
	unsigned size = 1;

	if (radar[x][y])
		return 0;

	if (floormap[x][y] == 9)
		return 0;

	radar[x][y] = 1;

	if (x > 0)
		size += explore(x - 1, y);
	if (x < (cur_h - 1))
		size += explore(x + 1, y);
	if (y > 0)
		size += explore(x, y - 1);
	if (y < (cur_w - 1))
		size += explore(x, y + 1);

	return size;
}

void add_bassin_size(unsigned int size)
{
	int i;

	for (i = 0; i < 3; ++i) {
		if (!sizes[i]) {
			sizes[i] = size;
			return;
		}
	}

	if (size > sizes[0]) {
		sizes[0] = size;
		return;
	}
	if (size > sizes[1]) {
		sizes[1] = size;
		return;
	}
	if (size > sizes[2]) {
		sizes[2] = size;
		return;
	}
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos, *tok;
	FILE *f;
	unsigned int i, j;

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
				LOWER(lower, floormap[i][j], floormap[i - 1][j]);
			if (j > 0)
				LOWER(lower, floormap[i][j], floormap[i][j - 1]);
			if (j < (cur_w - 1))
				LOWER(lower, floormap[i][j], floormap[i][j + 1]);
			if (i < (cur_h - 1))
				LOWER(lower, floormap[i][j], floormap[i + 1][j]);
			if (!lower)
				continue;
			memset(radar, 0, sizeof(radar));
			add_bassin_size(explore(i, j));
		}
	}

	printf("Biggest Bassins: ");
	for (i = 0; i < 3; ++i)
		printf("%d ", sizes[i]);
	printf("\n");
	printf("Result: %d\n", sizes[0] * sizes[1] * sizes[2]);

	return 0;
}
