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

unsigned long int extract_num(unsigned int *pos, unsigned int count)
{
	unsigned long val = 0;

	for (; count; --count) {
		val <<= 1;
		val |= bits[(*pos)++];
	}

	return val;
}

void porder(unsigned int order)
{
	while(order--)
		printf("  ");
}

unsigned long int decode_literal_val(unsigned int order, unsigned int *pos)
{
	unsigned long int val = 0;
	unsigned int cont;

	porder(order);

	printf("Literal Value: ");
	do {
		cont = bits[(*pos)++];

		val <<= 4;
		val |= extract_num(pos, 4);

	} while(cont);

	printf("%ld\n", val);

	return val;
}

unsigned long int decode(unsigned int order, unsigned int *pos);

unsigned long int operate(unsigned int type, unsigned long int val, unsigned int id, unsigned long int result)
{
	switch (type) {
	case 0:
		result += val;
		break;
	case 1:
		if (id == 0)
			result = val;
		else
			result *= val;
		break;
	case 2:
		if (id == 0)
			result = val;
		else if (val < result)
			result = val;
		break;
	case 3:
		if (id == 0)
			result = val;
		else if (val > result)
			result = val;
		break;
	case 5:
		if (id == 0)
			result = val;
		else if (id == 1 && result > val)
			result = 1;
		else if (id == 1)
			result = 0;
		break;
	case 6:
		if (id == 0)
			result = val;
		else if (id == 1 && result < val)
			result = 1;
		else if (id == 1)
			result = 0;
		break;
	case 7:
		if (id == 0)
			result = val;
		else if (id == 1 && result == val)
			result = 1;
		else if (id == 1)
			result = 0;
		break;
	}

	return result;
}

unsigned long int decode_operator(unsigned int order, unsigned int *pos, unsigned int type)
{
	unsigned int length_type_id = bits[(*pos)++];
	unsigned long int result = 0;
	unsigned int id = 0;

	if (!length_type_id) {
		unsigned int length = extract_num(pos, 15);
		unsigned int end = *pos + length;

		porder(order);
		printf("Sub-packets in %d next bits\n", length);

		do {
			result = operate(type, decode(order+1, pos), id++, result);
		} while (*pos < end);
	} else {
		int count = extract_num(pos, 11);

		porder(order);
		printf("Next %d Sub-packets\n", count);

		for (; count; --count) {
			result = operate(type, decode(order+1, pos), id++, result);
		}
	}


	porder(order);
	printf("Result: %ld\n", result);

	return result;
}

unsigned long int decode(unsigned int order, unsigned int *pos)
{
	unsigned int version, type;

	version = extract_num(pos, 3);

	type = extract_num(pos, 3);

	porder(order);
	printf("Packet Version %d Type %d\n", version, type);

	switch (type) {
	case LITERAL_VAL:
		return decode_literal_val(order, pos);
	default:
		return decode_operator(order, pos, type);
	}
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

	printf("Value: %ld\n", decode(0, &start));

	return 0;
}
