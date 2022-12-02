#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	unsigned long value, prev = 0;
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
		pos = fgets(temp, 255 - 1, f);
		if (!pos)
			break;

		value = strtoul(pos, NULL, 10);

		if (prev) {
			if (value > prev) {
				printf("%ld\t(increased)\n", value);
				num_inc++;
			} else if (value < prev)
				printf("%ld\t(decreased)\n", value);
			else
				printf("%ld\n", value);
		} else
			printf("%ld\t(N/A - no previous measurement)\n", value);

		prev = value;
	} while(pos);

	fclose(f);

	printf("%d measurements that are larger than the previous measurement.\n", num_inc);

	return 0;
}
