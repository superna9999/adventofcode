#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct map {
	unsigned int len;
	char *digits;
	char segs[7];
} digits[10] = {
			//	 a  b  c  d  e  f  g
	[0] = { 6, "abcefg", 	{1, 1, 1, 0, 1, 1, 1}},
	[1] = { 2, "cf", 	{0, 0, 1, 0, 0, 1, 0}},
	[2] = { 5, "acdeg", 	{1, 0, 1, 1, 1, 0, 1}},
	[3] = { 5, "acdfg", 	{1, 0, 1, 1, 0, 1, 1}},
	[4] = { 4, "bcdf", 	{0, 1, 1, 1, 0, 1, 0}},
	[5] = { 5, "abdfg", 	{1, 1, 0, 1, 0, 1, 1}},
	[6] = { 6, "abdefg", 	{1, 1, 0, 1, 1, 1, 1}},
	[7] = { 3, "acf", 	{1, 0, 1, 0, 0, 1, 0}},
	[8] = { 7, "abcdefg", 	{1, 1, 1, 1, 1, 1, 1}},
	[9] = { 6, "abcdfg", 	{1, 1, 1, 1, 0, 1, 1}},
};

void update_simple_probs(unsigned int num, unsigned int count, char *entry, int mapping[7][7])
{
	unsigned int i, j;

	for (i = 0; i < count; ++i) {
		for (j = 0; j < count; ++j) {
			mapping[digits[num].digits[i] - 'a'][entry[j] - 'a']++;
		}
	}
}

void update_mapping(char *entry, int mapping[7][7], unsigned int count)
{
	unsigned int i;

	for (i = 0; i < 10; ++i) {
		if (digits[i].len != count)
			continue;

		update_simple_probs(i, count, entry, mapping);
	}
}

void handle_digits(char *str, int mapping[7][7])
{
	char *tok;

	printf("'%s'\n", str);

	do {
		tok = strsep(&str, " \r\n");
		if (!strlen(tok))
			continue;

		update_mapping(tok, mapping, strlen(tok));
	} while (str);
}

int translate(char *str, char mapping[7])
{
	unsigned int i, j, k;
	char result[7], *tok;
	unsigned int value = 0;

	do {
		memset(result, 0, sizeof(result));
		k = 0;

		tok = strsep(&str, " \r\n");
		if (!strlen(tok))
			continue;

		for (i = 0; i < 7; ++i) {
			for (j = 0; j < strlen(tok); ++j) {
				if (tok[j] == mapping[i]) {
					result[k++] = 'a' + i;
				}
			}
		}
		result[k] = '\0';
		for (i = 0; i < 10; ++i) {
			if (strcmp(result, digits[i].digits) == 0) {
				value = (value * 10) + i;
				break;
			}
		}
		if (i == 10) {
			printf("Unknown digit :-(\n");
			return -1;
		}
	} while (str);

	printf("Found valuet %4d\n", value);

	return value;
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos, *tok;
	int mapping[7][7];
	char map[7];
	FILE *f;
	int i, j, max, count, cur, found = 0, pass;
	unsigned int sum = 0;

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
		memset(mapping, 0, sizeof(mapping));

		pos = fgets(temp, sizeof(temp) - 1, f);
		if (!pos)
			break;

		tok = strsep(&pos, "|");
		// tok | pos

		handle_digits(tok, mapping);
		
		for (pass = 0; pass < 7; ++pass) {
			for (i = 0; i < 7; ++i) {
				count = 0;
				cur = -1;
				max = -10;
				for (j = 0; j < 7; ++j) {
					if (mapping[i][j] > max) {
						count = 1;
						cur = j;
						max = mapping[i][j];
					} else if (count && mapping[i][j] == max) {
						count++;
					}
				}

				if (count == 1) {
					map[i] = 'a' + cur;
					found++;
					for (j = 0; j < 7; ++j) {
						mapping[j][cur]--;
					}
				}
			}
		}

		printf("Mapping:\n");
		for (i = 0; i < 7; ++i)
			printf("%c ==> %c\n", 'a' + i, map[i]);

		if (found < 7) {
			printf("Failed to find mapping...\n");
			break;
		}

		sum += translate(pos, map);

	} while(1);

	fclose(f);

	printf("Total: %d\n", sum);

	return 0;
}
