#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define GRID 10000

unsigned int grid[GRID][GRID];
unsigned int cur_w, cur_h;

#define MERGE(a, b) ((a) ? (a) : ((b) ? (b) : 0))

void run_fold(char *cmd)
{
	char *tok;
	unsigned long int along;
	unsigned int fold_up = 0;
	unsigned int x, y;

	tok = strsep(&cmd, "=");

	if (tok[strlen(tok) - 1] == 'y')
		fold_up = 1;

	along = strtoul(cmd, NULL, 10);

	printf("fold along %c=%ld\n", fold_up ? 'y' : 'x', along);

	if (fold_up) {
		for (y = 0; y < along; ++y) {
			for (x = 0; x <= cur_w; ++x) {
				grid[y][x] = MERGE(grid[y][x], grid[cur_h - y][x]);
				grid[cur_h - y][x] = 0;
			}
		}
		cur_h = along - 1;
	} else {
		for (y = 0; y <= cur_h; ++y) {
			for (x = 0; x < along; ++x) {
				grid[y][x] = MERGE(grid[y][x], grid[y][cur_w - x]);
				grid[y][cur_w - x] = 0;
			}
		}
		cur_w = along - 1;
	}
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos, *tok;
	unsigned long int x, y, dots = 0;;
	unsigned int do_cmd = 0;
	FILE *f;

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

		if (pos[0] == '\n') {
			do_cmd = 1;
			continue;
		}

		if (do_cmd) {
			run_fold(pos);
			// stop for now
			break;
		}

		tok = strsep(&pos, ",");
		
		x = strtoul(tok, NULL, 10);
		y = strtoul(pos, NULL, 10);

		grid[y][x] = 1;

		if (x > cur_w)
			cur_w = x;
		if (y > cur_h)
			cur_h = y;

	} while(1);

	fclose(f);

	for (y = 0; y <= cur_h; ++y) {
		for (x = 0; x <= cur_w; ++x) {
			dots += grid[y][x] ? 1 : 0;
		}
	}
	
	printf("Dots: %ld\n", dots);

	return 0;
}
