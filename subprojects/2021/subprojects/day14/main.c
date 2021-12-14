#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_ELEMS 100000
char elems[MAX_ELEMS];
unsigned int cur_elems;

struct addition {
	char c;
	unsigned int pos;
} additions[MAX_ELEMS];
unsigned int num_additions;

struct occur {
	char c;
	unsigned int count;
} occurs[MAX_ELEMS];
unsigned int num_occurs;

int compare_occurs(const void *a, const void *b)
{
	const struct occur *da = (const struct occur *) a;
	const struct occur *db = (const struct occur *) b;

	return (da->count > db->count) - (da->count < db->count);
}

void calc_occurs()
{
	unsigned i, j;

	num_occurs = 0;

	for (i = 0; i < cur_elems; ++i) {
		for (j = 0; j < num_occurs; ++j) {
			if (occurs[j].c == elems[i]) {
				occurs[j].count++;
				break;
			}
		}
		if (j == num_occurs) {
			occurs[num_occurs].c = elems[i];
			occurs[num_occurs].count = 1;
			num_occurs++;
		}
	}

	qsort(occurs, num_occurs, sizeof(struct occur), compare_occurs);

	printf("Min %c => %d\n", occurs[0].c, occurs[0].count); 
	printf("Max %c => %d\n", occurs[num_occurs - 1].c, occurs[num_occurs - 1].count);
	printf("Diff: %d\n", occurs[num_occurs - 1].count - occurs[0].count);
}

int compare_additions(const void *a, const void *b)
{
	const struct addition *da = (const struct addition *) a;
	const struct addition *db = (const struct addition *) b;

	return (da->pos > db->pos) - (da->pos < db->pos);
}

void add_elem(unsigned int pos, char c)
{
	additions[num_additions].c = c;
	additions[num_additions].pos = pos;
	num_additions++;
}

void commit_additions(void)
{
	unsigned i;
	unsigned shift = 0;

	qsort(additions, num_additions, sizeof(struct addition), compare_additions);

	for (i = 0; i < num_additions; ++i) {
		additions[i].pos += shift;
		memmove(&elems[additions[i].pos+1], &elems[additions[i].pos], cur_elems - additions[i].pos);
		elems[additions[i].pos] = additions[i].c;
		shift++;
		cur_elems++;
	}

	num_additions = 0;
	elems[cur_elems] = '\0';
}

void parse_template(char *str)
{
	size_t len = strlen(str);
	unsigned int i;

	for (i = 0; i < len; ++i) {
		if (str[i] == '\n')
			break;
		elems[i] = str[i];
	}
	cur_elems = i;
}

void insert_elem(char *rule)
{
	size_t len = strlen(rule);
	unsigned int i;
	char insert;

	insert = rule[len - 2];

	for (i = 0; i < (cur_elems - 1); ++i) {
		if (elems[i] == rule[0] && elems[i+1] == rule[1]) {
			add_elem(i + 1, insert);
		}
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

	for (step = 0; step < 10; ++step) {
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
