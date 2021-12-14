#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAP	10000

unsigned int map[MAP][MAP];
unsigned int x = MAP / 2, y = MAP / 2;
unsigned int x_r = MAP / 2, y_r = MAP / 2;

unsigned int handle_directions(char *input)
{
	size_t len = strlen(input);
	unsigned int i;
	unsigned int lucky = 0;
	unsigned int is_robot = 0;

	for (i = 0; i < len; ++i) {
		switch (input[i]) {
		case '>':
			if (is_robot)
				y_r++;
			else
				y++;
			break;
		case '<':
			if (is_robot)
				y_r--;
			else
				y--;
			break;
		case '^':
			if (is_robot)
				x_r--;
			else
				x--;
			break;
		case 'v':
			if (is_robot)
				x_r++;
			else
				x++;
			break;
		}
		if (is_robot) {
			if (!map[x_r][y_r])
				lucky++;
			map[x_r][y_r]++;
			is_robot = 0;
		} else {
			if (!map[x][y])
				lucky++;
			map[x][y]++;
			is_robot = 1;
		}
	}

	return lucky;
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;
	unsigned int lucky = 1;

	map[x][y] = 1;

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

		lucky += handle_directions(pos);
	} while(1);

	fclose(f);

	printf("Lucky Houses: %d\n", lucky);

	return 0;
}
