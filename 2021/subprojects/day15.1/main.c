#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

#define MAP 500

unsigned int map[MAP][MAP];
unsigned int radar[MAP][MAP];
unsigned int cur_w, cur_h;

struct position {
	unsigned int x;
	unsigned int y;
};
#define DIFF_POS(a, b) (((a).x == (b).x) && ((a).y == ((b).y)))

/********** SET ********/

#define MAX_SET (MAP * MAP)

struct pos_set {
	struct position entries[MAX_SET];
	unsigned int pos;
};

#define for_all_of_set(_i, _set, _entry)		\
	for (_i = 0, _entry = _set->entries[0];		\
	     _i < _set->pos;				\
	     ++_i, _entry = _set->entries[_i])

void pos_set_add(struct pos_set *set, struct position pos)
{
	unsigned int i;
	struct position entry;

	for_all_of_set(i, set, entry) {
		if (DIFF_POS(entry, pos))
			return;
	}

	set->entries[set->pos++] = pos;
	if (set->pos == MAX_SET) {
		printf("MAX_SET !\n");
	}
}

void pos_set_remove(struct pos_set *set, struct position entry)
{
	unsigned int i;
	struct position pos;

	for_all_of_set(i, set, pos) {
		if (DIFF_POS(pos, entry))
			break;
	}

	if (i == set->pos)
		return;

	for (; i < (set->pos - 1); ++i)
		set->entries[i] = set->entries[i + 1];
	set->pos--;
}

/********** MAP ********/

#define MAX_MAP (MAX_SET * 10)

struct pos_map {
	struct {
		struct position a;
		struct position b;
	} map[MAX_MAP];
	unsigned int pos;
};

void pos_map_add(struct pos_map *map, struct position a, struct position b)
{
	map->map[map->pos].a = a;
	map->map[map->pos++].b = b;
	if (map->pos == MAX_MAP) {
		printf("map MAX_MAP !\n");
	}
}

#define for_all_of_pos_map(_i, _map, _a)		\
	for (_i = 0, _a = _map->map[_i].a;		\
	     _i < _map->pos;				\
	     ++_i, _a = _map->map[_i].a)

unsigned int pos_map_get(struct pos_map *map, struct position a, struct position *b)
{
	unsigned int i;
	struct position pos;

	for_all_of_pos_map(i, map, pos) {
		if (DIFF_POS(a, pos)) {
			*b = map->map[i].b;
			return 1;
		}
	}

	return 0;
}

/********** SCORE ********/

struct score_map {
	unsigned int score[MAP][MAP];
};

void score_init(struct score_map *map, unsigned int score)
{
	unsigned int x, y;

	for (x = 0; x < MAP; ++x)
		for (y = 0; y < MAP; ++y)
			map->score[x][y] = score;

}

void score_map_add(struct score_map *map, struct position pos, unsigned int score)
{
	map->score[pos.x][pos.y] = score;
}

unsigned int score_map_get(struct score_map *map, struct position pos)
{

	return map->score[pos.x][pos.y];
}

#define CDIFF(a, b) ((a) < (b) ? ((b) - (a)) : ((a) - (b)))

unsigned int h(struct position cur, struct position goal)
{
	return CDIFF(cur.x, goal.x) + CDIFF(cur.y, goal.y);
}

struct position get_min_score(struct pos_set *openset, struct score_map *fscore)
{
	unsigned int lowest = UINT_MAX, i;
	struct position lowest_pos;
	struct position pos;

	for_all_of_set(i, openset, pos) {
		if (score_map_get(fscore, pos) < lowest) {
			lowest = score_map_get(fscore, pos);
			lowest_pos = pos;
		}
	}

	return lowest_pos;
}

unsigned int reconstruct_path(struct pos_map *camefrom, struct position cur)
{
	unsigned int risk = map[cur.x][cur.y];
	struct position b;

	radar[cur.x][cur.y] = 1;

	while (pos_map_get(camefrom, cur, &b)) {
		cur = b;
		
		radar[cur.x][cur.y] = 1;

		if (cur.x == 0 && cur.y == 0)
			break;

		risk += map[cur.x][cur.y];
	}

	return risk;
}

struct position neighbors[4];
struct pos_set openset;
struct pos_map camefrom;
struct score_map gscore, fscore;

unsigned int find_path(struct position start, struct position goal)
{
	unsigned int num_neighbors = 0;

	openset.pos = 0;
	camefrom.pos = 0;

	score_init(&gscore, UINT_MAX);
	score_init(&fscore, UINT_MAX);
	
	printf("from [%d:%d] to [%d:%d]\n", start.x, start.y, goal.x, goal.y);

	pos_set_add(&openset, start);
	score_map_add(&gscore, start, 0);
	score_map_add(&fscore, start, h(start, goal));

	while (openset.pos) {
		struct position current = get_min_score(&openset, &fscore);
		unsigned int i;

		if (DIFF_POS(current, goal))
			return reconstruct_path(&camefrom, current);

		pos_set_remove(&openset, current);

		num_neighbors = 0;
		if (current.x > 0) {
			neighbors[num_neighbors].x = current.x - 1;
			neighbors[num_neighbors++].y = current.y;
		}
		if (current.y > 0) {
			neighbors[num_neighbors].x = current.x;
			neighbors[num_neighbors++].y = current.y - 1;
		}
		if (current.x < (cur_h - 1)) {
			neighbors[num_neighbors].x = current.x + 1;
			neighbors[num_neighbors++].y = current.y;
		}
		if (current.y < (cur_w - 1)) {
			neighbors[num_neighbors].x = current.x;
			neighbors[num_neighbors++].y = current.y + 1;
		}
		
		for (i = 0; i < num_neighbors; ++i) {
			unsigned int tentative_gscore = score_map_get(&gscore, current) +
				map[neighbors[i].x][neighbors[i].y] * 10;

			if (tentative_gscore < score_map_get(&gscore, neighbors[i])) {
				pos_map_add(&camefrom, neighbors[i], current);
				score_map_add(&gscore, neighbors[i], tentative_gscore);
				score_map_add(&fscore, neighbors[i], tentative_gscore + h(neighbors[i], goal));
				pos_set_add(&openset, neighbors[i]);
			}
		}
	}

	return 0;
}

#define WRAP(_k) ((_k) > 9 ? 1 : (_k))

int main(int argc, char *argv[])
{
	char temp[1024], *tok;
	struct position start = {0, 0}, end;
	unsigned int i, j, z, risk;
	char *pos;
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

		tok = strsep(&pos, " \r\n");

		if (!cur_w)
			cur_w = strlen(tok);

		for (i = 0; i < strlen(tok); ++i)
			map[cur_h][i] = tok[i] - '0';

		cur_h++;
	} while(1);

	fclose(f);

	for (i = 0; i < cur_h; ++i) {
		for (z = 1; z < 5; ++z) {
			for (j = 0; j < cur_w; ++j) {
				map[i][(z * cur_w) + j] = WRAP(map[i][((z - 1) * cur_w) + j] + 1);
			}
		}
	}

	cur_w *= 5;

	for (z = 1; z < 5; ++z) {
		for (i = 0; i < cur_h; ++i) {
			for (j = 0; j < cur_w; ++j) {
				map[(z * cur_h) + i][j] = WRAP(map[((z - 1) * cur_h) + i][j] + 1);
			}
		}
	}

	cur_h *= 5;

	end.x = cur_h - 1;
	end.y = cur_w - 1;

	risk = find_path(start, end);

	printf("Shortest path: %d\n", risk);

	return 0;
}
