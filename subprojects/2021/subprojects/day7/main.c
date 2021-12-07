#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CRABS 2048

unsigned int crab[MAX_CRABS];
unsigned int fuel[MAX_CRABS];
unsigned int last_crab;

int main(int argc, char *argv[])
{
	char temp[16 * 1024];
	char *pos, *tok;
	unsigned long input;
	FILE *f;
	int i, j;

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
		pos = fgets(temp, sizeof(temp) - 1, f);
		if (!pos)
			break;

		do {
			tok = strsep(&pos, ",");
			input = strtoul(tok, NULL, 10);
			crab[input]++;
			if (input > last_crab)
				last_crab = input;
		} while (pos);

	} while(1);

	fclose(f);

	for (i = 0; i <= last_crab; ++i) {
		for (j = 0; j <= last_crab; ++j) {
			if (i == j || !crab[j])
				continue;
			fuel[i] += (i > j ? i - j : j - i) * crab[j];
		}
	}

	j = (1 << 32) - 1;
	for (i = 0; i <= last_crab; ++i) {
		if (fuel[i] < j)
			j = fuel[i];
	}

	printf("Min fuel: %d\n", j);

	return 0;
}
