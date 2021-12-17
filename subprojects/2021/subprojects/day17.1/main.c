#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define TARGET_MIN	0
#define TARGET_MAX	1
int target_x[2];
int target_y[2];

void load_target(char *str)
{
	sscanf(str, "target area: x=%d..%d, y=%d..%d",
	       &target_x[TARGET_MIN], &target_x[TARGET_MAX],
	       &target_y[TARGET_MIN], &target_y[TARGET_MAX]);

	printf("target: x [%d, %d] y [%d, %d]\n",
		target_x[TARGET_MIN], target_x[TARGET_MAX],
		target_y[TARGET_MIN], target_y[TARGET_MAX]);
}

int simulate(int velocity_x, int velocity_y)
{
	int x = 0;
	int y = 0;

	do {
		x += velocity_x;
		y += velocity_y;

		if (velocity_x > 0)
			velocity_x--;
		else if (velocity_x < 0)
			velocity_x++;

		velocity_y--;

		if (x >= target_x[TARGET_MIN] && x <= target_x[TARGET_MAX] &&
		    y >= target_y[TARGET_MIN] && y <= target_y[TARGET_MAX])
			return 1;

		if (x > target_x[TARGET_MAX] || y < target_y[TARGET_MIN])
			break;
	} while(1);

	return 0;
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;
	unsigned int i, count = 0;
	int ret, j;

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

		load_target(pos);

		break;

	} while(1);

	fclose(f);

	for (i = 1; i < 1000; ++i) {
		for (j = -1000; j < 1000; ++j) {
			ret = simulate(i, j);
			if (ret > 0)
				count++;
		}
	}

	printf("Possible Initial Velocities: %d\n", count);

	return 0;
}
