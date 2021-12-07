#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DAYS 300

uint64_t birthdays[MAX_DAYS];
uint64_t population;

void day(unsigned int date)
{
	int i;

	for (i = date + 9; i < MAX_DAYS; i += 7)
		birthdays[i] += birthdays[date];
	population += birthdays[date];
}

int main(int argc, char *argv[])
{
	char temp[2048];
	char *pos, *tok;
	FILE *f;
	unsigned int i, days, age;

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
		pos = fgets(temp, 2048 - 1, f);
		if (!pos)
			break;

		do {
			tok = strsep(&pos, ",");
			age = strtoul(tok, NULL, 10);
			for (i = 0; i < MAX_DAYS; i += 7)
				birthdays[age + i]++;
			population++;
		} while (pos);

	} while(1);

	fclose(f);

	printf("Initial state: %10ld lanterfishes\n", population);

	for (days = 0; days < 256; ++days) {
		day(days);
		printf("After %2d days: %10ld lanterfishes\n", days, population);
	}

	printf("%ld lanterfishes\n", population);

	return 0;
}
