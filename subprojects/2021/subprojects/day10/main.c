#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_CHUNKS 1000

char stack[MAX_CHUNKS];

int check_line(char *str)
{
	unsigned int i, pos = 0;
	unsigned int len = strlen(str);
	char expected;
	unsigned int score;

	for (i = 0; i < len; ++i) {
		switch (str[i]) {
		case '(':
		case '[':
		case '{':
		case '<':
			stack[pos++] = str[i];
			break;
		case ')':
		case ']':
		case '}':
		case '>':
			if (pos == 0) {
				printf("Unexpected closing '%c' (%d/%d)\n", str[i], i, len);
				return -1;
			}
			switch (stack[pos - 1]) {
			case '(':
				expected = ')';
				break;
			case '[':
				expected = ']';
				break;
			case '{':
				expected = '}';
				break;
			case '<':
				expected = '>';
				break;
			}
			switch (str[i]) {
			case ')':
				score = 3;
				break;
			case ']':
				score = 57;
				break;
			case '}':
				score = 1197;
				break;
			case '>':
				score = 25137;
				break;
			}
			if (str[i] != expected) {
				printf("Expected %c, but found %c instead (%d/%d)\n", expected, str[i], i, len);
				return score;
			} else
				--pos;
			break;
		case '\n':
			if (pos > 0) {
				printf("Incomplete line, expected %d closings\n", pos);
				return -1;
			}
			break;
		default:
			printf("Invalid char '%c'\n", str[i]);
			return -1;
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;
	unsigned int total = 0;
	int score;

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

		printf("%s", pos);

		score = check_line(pos);
		if (score >= 0)
			total += score;
		printf("Score: %d\n", score);
	} while(1);

	fclose(f);

	printf("Score Total: %d\n", total);

	return 0;
}
