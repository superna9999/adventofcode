#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define GRID 10
#define STEPS 2000

unsigned int grid[GRID][GRID];

void flash(unsigned int x, unsigned int y)
{
	if (grid[x][y] < 10)
		++grid[x][y];

	if (grid[x][y] < 10)
		return;

	if (grid[x][y] == 10)
		grid[x][y]++;
	else if (grid[x][y] == 11)
		return;
	
	//printf("x %d y %d -> %d\n", x, y, grid[x][y]);

	if (x > 0) {
		if (y > 0)
			flash(x-1, y-1);
		flash(x-1, y);
		if (y < (GRID - 1))
			flash(x-1, y+1);
	}
	if (y > 0)
		flash(x, y-1);
	if (y < (GRID - 1))
		flash(x, y+1);
	if (x < (GRID - 1)) {
		if (y > 0)
			flash(x+1, y-1);
		flash(x+1, y);
		if (y < (GRID - 1))
			flash(x+1, y+1);
	}
}

unsigned int step(void)
{
	unsigned int x, y;
	unsigned int flashes = 0;

	for (x = 0; x < GRID; ++x) {
		for (y = 0; y < GRID; ++y) {
			grid[x][y]++;
		}
	}

	for (x = 0; x < GRID; ++x) {
		for (y = 0; y < GRID; ++y) {
			if (grid[x][y] == 10)
				flash(x, y);
		}
	}

	for (x = 0; x < GRID; ++x) {
		for (y = 0; y < GRID; ++y) {
			if (grid[x][y] > 10) {
				grid[x][y] = 0;
				flashes++;
				printf("*");
			}
			else
				printf("%d", grid[x][y]);
		}
		printf("\n");
	}

	return flashes;
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;
	unsigned int line = 0, x, y, steps, flashes = 0;

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

		if ((strlen(pos) - 1) < GRID) {
			fprintf(stderr, "Invalid line %d size\n", line);
			fclose(f);
			return 1;
		}

		for (y = 0; y < GRID; ++y)
			grid[line][y] = pos[y] - '0';

		++line;
	} while(1);

	fclose(f);

	for (x = 0; x < GRID; ++x) {
		for (y = 0; y < GRID; ++y) {
			printf("%d", grid[x][y]);
		}
		printf("\n");
	}

	for (steps = 0; steps < STEPS; ++steps) {
		printf("Step: %d\n", steps);
		if (step() == GRID*GRID)
			break;
	}
	
	printf("All Flashes at step: %d\n", steps + 1);

	return 0;
}
