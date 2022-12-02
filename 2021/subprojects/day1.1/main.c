#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define N_WINDOWS 3

struct _state {
	unsigned long counters[N_WINDOWS];
	unsigned int windows[N_WINDOWS];
	unsigned int started[N_WINDOWS];
} state = {
	.counters = {0, 0, 0},
	.windows = {0, 0, 0},
};

int main(int argc, char *argv[])
{
	unsigned long meas, value, prev = 0;
	unsigned int num_inc = 0;
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
		unsigned int ready = 0;
		int i;

		pos = fgets(temp, 255 - 1, f);
		if (!pos)
			break;

		value = strtoul(pos, NULL, 10);

		for (i = 0 ; i < N_WINDOWS ; ++i) {
			if (state.windows[i] < N_WINDOWS) {
				state.counters[i] += value;
				state.windows[i]++;
				if (!state.started[i]) {
					state.started[i] = 1;
					break;
				}
				if (state.windows[i] == N_WINDOWS) {
					meas = state.counters[i];
					state.counters[i] = 0;
					state.windows[i] = 0;
					ready = 1;
				}
			}
		}

		if (!ready)
			continue;

		if (prev) {
			if (meas > prev) {
				printf("%ld\t(increased)\n", meas);
				num_inc++;
			} else if (meas < prev)
				printf("%ld\t(decreased)\n", meas);
			else
				printf("%ld\n", meas);
		} else
			printf("%ld\t(N/A - no previous measurement)\n", meas);

		prev = meas;
	} while(pos);

	fclose(f);

	printf("%d measurements that are larger than the previous measurement.\n", num_inc);

	return 0;
}
