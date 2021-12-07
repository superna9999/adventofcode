#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAXBITS 255

int main(int argc, char *argv[])
{
	unsigned int gamma = 0, epsilon = 0;
	unsigned int bitcounts[MAXBITS];
	unsigned int bit1counts[MAXBITS];
	unsigned int entries = 0;
	char temp[255];
	char *pos;
	FILE *f;
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

	memset(bitcounts, 0, sizeof(bitcounts));
	memset(bit1counts, 0, sizeof(bit1counts));

	do {
		pos = fgets(temp, 255 - 1, f);
		if (!pos)
			break;

		for (i = 0 ; i < strlen(pos) ; ++i) {
			if (pos[i] == '1') {
				bit1counts[i]++;
				bitcounts[i]++;
			} else if (pos[i] == '0') {
				bitcounts[i]++;
			}
		}

		++entries;
	} while(pos);

	fclose(f);

	for (i = 0 ; i < MAXBITS ; ++i) {
		if (bitcounts[i] != entries)
			break;
		gamma <<= 1;
		epsilon <<= 1;
		if (bit1counts[i] > (entries / 2))
			gamma |= 1;
		else
			epsilon |= 1;
	}

	printf("Gamma %d Epsilon %d => %d\n", gamma, epsilon, gamma * epsilon);

	return 0;
}
