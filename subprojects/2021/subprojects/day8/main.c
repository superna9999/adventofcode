#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct map {
	unsigned int len;
	char *digits;
	unsigned int segs[7];
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

int find_possible_matches(char *entry)
{
	size_t len = strlen(entry);
	int i, matches = 0;

	for (i = 0; i < 10; ++i)
		if (len == digits[i].len)
			matches++;

	return matches;
}

int handle_digits(char *str)
{
	int matches = 0;
	char *tok;

	do {
		tok = strsep(&str, " \r\n");
		if (!strlen(tok))
			continue;
		if (find_possible_matches(tok) == 1)
			matches++;
	} while (str);

	return matches;
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos, *tok;
	unsigned int pre = 0, post = 0;
	FILE *f;

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

		tok = strsep(&pos, "|");
		// tok | pos

		pre += handle_digits(tok);
		post += handle_digits(pos);

	} while(1);

	fclose(f);

	printf("Pre: %d matches\n", pre);
	printf("Post: %d matches\n", post);

	return 0;
}
