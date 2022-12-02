#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define STEPS 40

#define MAX_CHAR 26
uint64_t pairs[MAX_CHAR][MAX_CHAR];
uint64_t additions[MAX_CHAR][MAX_CHAR];
uint64_t occurs[MAX_CHAR];

void calc_occurs()
{
	unsigned int i;
	uint64_t min_occur = ULONG_MAX, max_occur = 0;
	char min_c = '#', max_c = '#';

	for (i = 0; i < MAX_CHAR; ++i) {
		if (occurs[i] > max_occur) {
			max_occur = occurs[i];
			max_c = 'A' + i;
		}
		if (occurs[i] && occurs[i] < min_occur) {
			min_occur = occurs[i];
			min_c = 'A' + i;
		}
	}

	printf("Min %c => %ld\n", min_c, min_occur); 
	printf("Max %c => %ld\n", max_c, max_occur);
	printf("Diff: %ld\n", max_occur - min_occur);
}

void commit_additions(void)
{
	unsigned int i, j;

	for (i = 0; i < MAX_CHAR; ++i)
		for (j = 0; j < MAX_CHAR; ++j) {
			pairs[i][j] += additions[i][j];
		}

	memset(additions, 0, sizeof(additions));
}

void parse_template(char *str)
{
	size_t len;
	char *template;
	unsigned int i;

	template = strsep(&str, "\n\t ");
	len = strlen(template);

	printf("Template: %s\n", template);

	for (i = 0; i < (len - 1); ++i) {
		if (template[i] < 'A' || template[i] > 'Z' ||
		    template[i] < 'A' || template[i] > 'Z') {
			printf("Invalid template @ %d (%s)\n", i, template);
			return;
		}
		occurs[template[i] - 'A']++;
		pairs[template[i] - 'A'][template[i + 1] - 'A']++;
	}
	occurs[template[i] - 'A']++;
}

void insert_elem(char *rule)
{
	size_t len = strlen(rule);
	uint64_t count;
	char insert;

	insert = rule[len - 2];

	if (rule[0] < 'A' || rule[0] > 'Z' ||
	    rule[1] < 'A' || rule[1] > 'Z' ||
	    insert < 'A' || insert > 'Z' )
			return;

	count = pairs[rule[0] - 'A'][rule[1] - 'A'];

	if (count) {
		additions[rule[0] - 'A'][insert - 'A'] += count;
		additions[insert - 'A'][rule[1] - 'A'] += count;
		pairs[rule[0] - 'A'][rule[1] - 'A'] = 0;
		occurs[insert - 'A'] += count;
	}
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;
	unsigned int template_loaded = 0;
	long int offset_rules = -1;
	unsigned int step;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "Failed to open file '%s' (%s)\n", argv[1], strerror(errno));
		return 1;
	}

	for (step = 0; step < STEPS; ++step) {
		do {
			pos = fgets(temp, sizeof(temp) - 1, f);
			if (!pos)
				break;

			if (!strlen(pos))
				continue;

			if (!template_loaded) {
				parse_template(pos);
				offset_rules = ftell(f);
				template_loaded = 1;
			} else
				insert_elem(pos);
		} while(1);

		fseek(f, offset_rules, SEEK_SET);

		commit_additions();

		printf("After step %d:\n", step + 1);
		calc_occurs();
	}

	fclose(f);


	return 0;
}
