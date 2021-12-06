#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAP_W	1000
#define MAP_H	1000

unsigned int map[MAP_H][MAP_W];

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define POS(a) ((a) < 0 ? -(a) : (a))

void draw_line(unsigned int x1, unsigned y1, unsigned int x2, unsigned int y2)
{
	unsigned int i, j;
	
	if (x1 == x2) {
		for (i = MIN(y1, y2); i <= MAX(y1, y2); ++i) {
			map[i][x1]++;
		}
	} else if (y1 == y2) {
		for (i = MIN(x1, x2); i <= MAX(x1, x2); ++i) {
			map[y1][i]++;
		}
	} else {
		unsigned int count = POS((int)x1 - (int)x2) + 1;
		i = y1;
		j = x1;

		do {
			map[i][j]++;
			if (y2 < y1)
				i--;
			else
				i++;
			if (x2 < x1)
				j--;
			else
				j++;
			count--;
		} while (count);
	}
}

void handle_coords(char *str)
{
	unsigned x1, y1, x2, y2;
	char *pos = str, *tok;

	tok = strsep(&pos, ",");
	x1 = strtoul(tok, NULL, 10);
	tok = strsep(&pos, " ");
	y1 = strtoul(tok, NULL, 10);
	tok = strsep(&pos, " ");
	tok = strsep(&pos, ",");
	x2 = strtoul(tok, NULL, 10);
	y2 = strtoul(pos, NULL, 10);

	draw_line(x1, y1, x2, y2);
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;
	int i, j;
	unsigned int overlap = 0;

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

		handle_coords(pos);

	} while(pos);

	fclose(f);

	for (i = 0; i < MAP_H; ++i)
		for (j = 0; j < MAP_W; ++j)
			if (map[i][j] > 1)
				overlap++;

	printf("Overlap: %d\n", overlap);

	return 0;
}
