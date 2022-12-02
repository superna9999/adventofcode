#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <alloca.h>

#define MAX_CAVES 100

struct cave {
	char *name;
	unsigned int is_large;
	struct cave *next[MAX_CAVES];
	unsigned num_next;
	struct cave *prev[MAX_CAVES];
	unsigned num_prev;
} caves[MAX_CAVES] = {
	{ "start", 0, {NULL}, 0, {NULL}, 0},
	{ "end", 0, {NULL}, 0, {NULL}, 0},
};
unsigned int num_caves = 2;

struct cave *start = &caves[0], *end = &caves[1];

int path_small_cave_max_visit(struct cave **path, unsigned int path_pos)
{
	unsigned int max = 1, i, j;

	for (i = 0; i < path_pos; ++i) {
		unsigned int count = 1;

		if (path[i]->is_large)
			continue;

		for (j = 0; j < path_pos; ++j) {
			if (i == j)
				continue;
			if (path[i] == path[j])
				count++;
		}
		if (count > max)
			max = count;
	}

	return max;
}

int explore(struct cave *s, struct cave *e, struct cave **path, unsigned int path_pos)
{
	unsigned int j, ret, paths = 0;

	if (s == e) {
		printf("path: ");
		for (j = 0; j < path_pos; ++j)
			printf("%s,", path[j]->name);
		printf("%s\n", e->name);
		return 1;
	}

	if (path_pos && strcmp(s->name, "start") == 0)
		return 0;

	if (!path)
		path = alloca(sizeof(struct cave *) * MAX_CAVES);

	if (!s->is_large) {
		if (path_small_cave_max_visit(path, path_pos) > 1) {
			for (j = 0; j < path_pos; ++j)
				if (s == path[j]) {
					return 0;
				}
		}
	}

	path[path_pos++] = s;

	for (j = 0; j < s->num_prev; ++j) {
		ret = explore(s->prev[j], e, path, path_pos);
		if (ret > 0)
			paths += ret;
	}

	for (j = 0; j < s->num_next; ++j) {
		ret = explore(s->next[j], e, path, path_pos);
		if (ret > 0)
			paths += ret;
	}

	return paths;
}

void parse_cave_path(char *str)
{
	char *tok1, *tok2;
	struct cave *cave1 = NULL, *cave2 = NULL;
	unsigned int i;
       
	tok1 = strsep(&str, "-");
	tok2 = strsep(&str, "\n");

	for (i = 0; i < num_caves; ++i) {
		if (!cave1 && strcmp(tok1, caves[i].name) == 0) {
			cave1 = &caves[i];
		}
		if (!cave2 && strcmp(tok2, caves[i].name) == 0) {
			cave2 = &caves[i];
		}
	}

	if (!cave1) {
		cave1 = &caves[num_caves++];
		cave1->name = strdup(tok1);
		if (tok1[0] >= 'A' && tok1[0] <= 'Z')
			cave1->is_large = 1;
	}

	if (!cave2) {
		cave2 = &caves[num_caves++];
		cave2->name = strdup(tok2);
		if (tok2[0] >= 'A' && tok2[0] <= 'Z')
			cave2->is_large = 1;
	}

	for (i = 0; i < cave1->num_next; ++i) {
		if (cave1->next[i] == cave2)
			break;
	}
	if (i == cave1->num_next)
		cave1->next[cave1->num_next++] = cave2;
	
	for (i = 0; i < cave2->num_prev; ++i) {
		if (cave2->prev[i] == cave1)
			break;
	}
	if (i == cave2->num_prev)
		cave2->prev[cave2->num_prev++] = cave1;
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;
	unsigned int i, j, paths;

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

		parse_cave_path(pos);

	} while(1);

	fclose(f);

	for (i = 0; i < num_caves; ++i) {
		if (!caves[i].name) {
			fprintf(stderr, "Invalid cave name %d\n", i);
			continue;
		}
		printf("{");
		for (j = 0; j < caves[i].num_prev; ++j) {
			if (!caves[i].prev[j]) {
				fprintf(stderr, "Invalid cave %d prev %d\n", i, j);
				continue;
			}
			if (caves[i].prev[j]->is_large)
				printf("<%s> ", caves[i].prev[j]->name);
			else
				printf("'%s' ", caves[i].prev[j]->name);
		}
		printf("} <- [%s] -> {", caves[i].name);
		for (j = 0; j < caves[i].num_next; ++j) {
			if (!caves[i].next[j]) {
				fprintf(stderr, "Invalid cave %d next %d\n", i, j);
				continue;
			}
			if (caves[i].next[j]->is_large)
				printf("<%s> ", caves[i].next[j]->name);
			else
				printf("'%s' ", caves[i].next[j]->name);
		}
		printf("}\n");
	}

	paths = explore(start, end, NULL, 0);

	printf("Paths: %d\n", paths);

	return 0;
}
