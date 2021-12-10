#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_CHUNKS 1000
#define MAX_LINES 1000

char stack[MAX_CHUNKS];
unsigned long int scores[MAX_LINES];

long int check_line(char *str)
{
	unsigned int i, pos = 0;
	unsigned int len = strlen(str);
	char expected;
	unsigned long int score;

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
			if (str[i] != expected) {
				printf("Expected %c, but found %c instead (%d/%d)\n", expected, str[i], i, len);
				return -1;
			} else
				--pos;
			break;
		case '\n':
			if (pos > 0) {
				printf("Incomplete line, expected %d closings: ", pos);
				score = 0;
				do {
					score *= 5;
					switch (stack[--pos]) {
					case '(':
						printf(")");
						score += 1;
						break;
					case '[':
						printf("]");
						score += 2;
						break;
					case '{':
						printf("}");
						score += 3;
						break;
					case '<':
						printf(">");
						score += 4;
						break;
					}

				} while (pos);
				printf(" (score %ld)\n", score);
				return score;
			}
			break;
		default:
			printf("Invalid char '%c'\n", str[i]);
			return -1;
		}
	}

	return 0;
}

int compare_scores (const void *a, const void *b)
{
	const unsigned long int *da = (const unsigned long int *) a;
	const unsigned long int *db = (const unsigned long int *) b;

	return (*da > *db) - (*da < *db);
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;
	unsigned lines = 0;
	long int score;

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
		if (score >= 0) {
			scores[lines++] = score;;
		}
	} while(1);

	fclose(f);

	qsort(scores, lines, sizeof(scores[0]), compare_scores);

	printf("Middle Score: %ld\n", scores[lines / 2]);

	return 0;
}
