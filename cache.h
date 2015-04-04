/* Cache Data Structure */

#pragma once

struct block {
	int tag;
	int valid;
};

struct cache {
	int block_size;
	int cache_size;
	int assoc;
	int hit_time;
	int miss_time;
	struct block **buf;	/* Indexed by cache index then way */
};

extern struct cache l1_d;
extern struct cache l1_i;
extern struct cache l2;

void init_cache(struct cache *cache);

void cache_write(struct cache *cache);
void cache_read(struct cache *cache);
void l2_l1_transfer(struct cache *l1, struct cache *l2, int l2_transfer_time, int l2_bus_width);
