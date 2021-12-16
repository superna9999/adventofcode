#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * 0:2 version
 * 3:5 type ID
 */

enum types {
	LITERAL_VAL = 4,
};

/* Literal Value
 * Literal value packets encode a single binary number.
 * To do this, the binary number is padded with leading zeroes until its length
 * is a multiple of four bits, and then it is broken into groups of four bits.
 * Each group is prefixed by a 1 bit except the last group, which is prefixed by a 0 bit.
 * These groups of five bits immediately follow the packet header.
 */

/* Operator
 */

#define MAX_BITS 10000

unsigned int bits[MAX_BITS];
unsigned int length;

unsigned int extract_num(unsigned int *pos, unsigned int count)
{
	unsigned int val = 0;

	for (; count; --count) {
		val <<= 1;
		val |= bits[(*pos)++];
	}

	return val;
}

unsigned int decode_literal_val(unsigned int *pos)
{
	unsigned int val = 0;
	unsigned int cont;

	printf("Literal Value: ");
	do {
		cont = bits[(*pos)++];

		val <<= 4;
		val |= extract_num(pos, 4);

	} while(cont);

	printf("%d\n", val);

	return val;
}

unsigned int decode(unsigned int *pos);

unsigned int decode_operator(unsigned int *pos)
{
	unsigned int length_type_id = bits[(*pos)++];
	unsigned int sum = 0;

	if (!length_type_id) {
		unsigned int length = extract_num(pos, 15);
		unsigned int end = *pos + length;

		printf("Sub-packets in %d next bits\n", length);

		do {
			sum += decode(pos);
		} while (*pos < end);
	} else {
		int count = extract_num(pos, 11);

		printf("Next %d Sub-packets\n", count);

		for (; count; --count) {
			sum += decode(pos);
		}
	}

	return sum;
}

unsigned int decode(unsigned int *pos)
{
	unsigned int version, type, val;

	version = extract_num(pos, 3);

	type = extract_num(pos, 3);

	printf("Packet Version %d Type %d\n", version, type);

	switch (type) {
	case LITERAL_VAL:
		decode_literal_val(pos);
		break;
	default:
		return version + decode_operator(pos);
	}

	return version;
}

void strtobits(char *str)
{
	unsigned int len = strlen(str);
	char strconv[] = "X";
	unsigned int i, val;
	int j;

	for (i = 0; i < len; ++i) {
		if (str[i] == '\n')
			break;

		strconv[0] = str[i];
		val = strtoul(strconv, NULL, 16);

		for (j = 3; j >= 0; --j)
			bits[length++] = (val & (1 << j) ? 1 : 0);
	}

	for (i = 0; i < length; ++i)
		printf("%d", bits[i]);
	printf("\n");
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;
	unsigned int start = 0;

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

		strtobits(pos);

	} while(1);

	fclose(f);

	printf("Value: %d\n", decode(&start));

	return 0;
}
