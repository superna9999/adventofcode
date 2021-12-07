#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LF	1000000

uint8_t lanternfish[MAX_LF];
unsigned long last = 0;

int birth(void)
{
	if (last == MAX_LF) {
		printf("unable to birth\n");
		return -1;
	}

	lanternfish[last++] = 8;

	return 0;
}

int day(void)
{
	unsigned long count = last;
	unsigned long i;

	for (i = 0; i < count; ++i) {
		switch (lanternfish[i]) {
		case 0:
			lanternfish[i] = 6;
			if (birth() < 0)
				return -1;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			lanternfish[i]--;
			break;
		default:
			printf("oops %ld = %d\n", i, lanternfish[i]);
			return -1;
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	char temp[2048];
	char *pos, *tok;
	FILE *f;
	unsigned int i, days;

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
			lanternfish[last++] = strtoul(tok, NULL, 10);
		} while (pos);

	} while(1);

	fclose(f);

	printf("Initial state: ");
	for (i = 0; i < last; ++i)
		printf("%d, ", lanternfish[i]);
	printf("\n");

	for (days = 0; days < 80; ++days) {
		if (day() < 0)
			return 1;
		printf("After %2d days: %10ld lanterfishes\n", days, last);
	}

	printf("%ld lanterfishes\n", last);

	return 0;
}
