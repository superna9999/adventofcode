#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define GRID_W 5
#define GRID_H 5

#define MAX_GRIDS 255

struct _grid {
	unsigned int numbers[GRID_H][GRID_W];
	unsigned int drawn[GRID_H][GRID_W];
} grids[MAX_GRIDS];

unsigned int load_grids(FILE *f)
{
	char temp[255];
	char *line;
	char *pos, *tok;
	int grid = 0, ln, col;

	do {
		for (ln = 0; ln < GRID_H; ++ln) {
			line = fgets(temp, 255 - 1, f);
			if (!line)
				break;

			col = 0;
			pos = line;
			do {
				tok = strsep(&pos, " ");
				if (tok[0] == NULL)
					continue;
				grids[grid].numbers[ln][col] = strtoul(tok, NULL, 10);
				++col;
			} while (pos && col < GRID_W);
		}
		++grid;

		// Read empty line
		line = fgets(temp, 255 - 1, f);
	} while (line);
	
	{
		int i;

		for (i = 0; i < grid; ++i) {
			for (ln = 0; ln < GRID_H; ++ln) {
				for (col = 0; col < GRID_W; ++col) {
					printf("%2d ", grids[i].numbers[ln][col]);
				}
				printf("\n");
			}
			printf("\n");
		}
	}

	return grid;
}

unsigned int grid_sum(int grid)
{
	unsigned int ln, col, sum = 0;

	for (ln = 0 ; ln < GRID_H; ++ln)
		for (col = 0; col < GRID_W; ++col)
			if (!grids[grid].drawn[ln][col])
				sum += grids[grid].numbers[ln][col];

	printf("Sum: %d\n", sum);

	return sum;
}

int mark_grid(int grid, unsigned int number)
{
	unsigned int ln, col, sum = 0;
	
	printf("==============================\n");
	printf("Grid %d Draw %d\n", grid, number);

	for (ln = 0 ; ln < GRID_H; ++ln) {
		for (col = 0; col < GRID_W; ++col) {
			if (grids[grid].numbers[ln][col] == number)
				grids[grid].drawn[ln][col]++;
		}
	}

	{
		for (ln = 0; ln < GRID_H; ++ln) {
			for (col = 0; col < GRID_W; ++col) {
				if (grids[grid].drawn[ln][col])
					printf("[%02d] ", grids[grid].numbers[ln][col]);
				else
					printf(" %02d  ", grids[grid].numbers[ln][col]);
			}
			printf("\n");
		}
		printf("\n");
	}

	for (col = 0; col < GRID_W; ++col) {
		for (ln = 0 ; ln < GRID_H; ++ln) {
			if (grids[grid].drawn[ln][col] == 0)
				break;
		}

		if (ln == GRID_W) {
			printf("Winner ! Grid %d Draw %d\n", grid, number);
			return grid_sum(grid) * number;
		}
	}

	for (ln = 0 ; ln < GRID_H; ++ln) {
		for (col = 0; col < GRID_W; ++col) {
			if (grids[grid].drawn[ln][col] == 0)
				break;
		}

		if (col == GRID_W) {
			printf("Winner ! Grid %d Draw %d\n", grid, number);
			return grid_sum(grid) * number;
		}
	}

	return -1;
}

unsigned int run_draws(char *draws, unsigned int grids)
{
	char *data;
	char *pos, *tok;
	unsigned grid;
	int ret;

	data = strdup(draws);

	pos = data;
	do {
		tok = strsep(&pos, ",");
		for (grid = 0 ; grid < grids ; ++grid) {
			ret = mark_grid(grid, strtoul(tok, NULL, 10));
			if (ret >= 0)
				return ret;
		}
	} while (pos);

	free(data);

	return 0;
}

int main(int argc, char *argv[])
{
	unsigned int grids;
	char temp[1024];
	char draws[1024];
	FILE *f;
	int ret;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "Failed to open file '%s' (%m)\n", argv[1]);
		return 1;
	}

	if (!fgets(draws, 1024 - 1, f)) {
		fprintf(stderr, "Failed to read input numbers (%m)\n", argv[1]);
		return 1;
	}

	// Read empty line
	fgets(temp, 1024 - 1, f);

	grids = load_grids(f);

	fclose(f);
	
	ret = run_draws(draws, grids);
	
	printf("Score: %d\n", ret);

	return 0;
}
