#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAXBITS 255

int pass(FILE *f, unsigned int bit, unsigned int *pattern, unsigned oxygen)
{
	unsigned int bit1counts = 0;
	unsigned int entries = 0;
	char temp[255];
	char *pos;
	size_t i;

	rewind(f);
	
	do {
		int skip_entry = 0;

		pos = fgets(temp, 255 - 1, f);
		if (!pos)
			break;

		for (i = 0 ; i <= bit ; ++i) {
			if (i == bit) {
				if (pos[i] == '1') {
					bit1counts++;
				} else if (pos[i] != '0')
					return -1; /* not a bit */
				skip_entry = 0;
			} else {
				if ((pattern[i] && pos[i] != '1') ||
				    (!pattern[i] && pos[i] != '0')) {
					skip_entry = 1;
					break;
				}
			}
		}

		if (skip_entry)
			continue;

		++entries;
	} while(pos);
	
	if (oxygen) {
		if (entries == 1)
			return bit1counts;
		if ((bit1counts * 2) > entries || (bit1counts * 2) == entries)
			return 1;

		return 0;
	} else {
		if (entries == 1)
			return bit1counts;
		if ((bit1counts * 2) > entries || (bit1counts * 2) == entries)
			return 0;

		return 1;
	}
}

int main(int argc, char *argv[])
{
	unsigned int oxygen = 0, co2 = 0;
	unsigned int pattern[MAXBITS];
	unsigned int curbit = 0;
	FILE *f;
	int ret;
	size_t i;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "Failed to open file '%s' (%m)\n", argv[1]);
		return 1;
	}

	memset(pattern, 0, sizeof(pattern));

	do {
		ret = pass(f, curbit, pattern, 1);
		if (ret < 0)
			break;

		pattern[curbit++] = ret;
	} while (ret >= 0);

	for (i = 0 ; i < curbit ; ++i) {
		oxygen <<= 1;
		if (pattern[i])
			oxygen |= 1;
	}

	curbit = 0;
	memset(pattern, 0, sizeof(pattern));

	do {
		ret = pass(f, curbit, pattern, 0);
		if (ret < 0)
			break;

		pattern[curbit++] = ret;
	} while (ret >= 0);

	for (i = 0 ; i < curbit ; ++i) {
		co2 <<= 1;
		if (pattern[i])
			co2 |= 1;
	}

	fclose(f);

	printf("Oxygen %d C02 %d => %d\n", oxygen, co2, oxygen * co2);

	return 0;
}
