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

void init_l1_cache(struct l1_config *l1, struct cache *cache);
void init_l2_cache(struct l2_config *l2, struct cache *cache);

void cache_write(struct cache *cache);
void cache_read(struct cache *cache);
void l2_l1_transfer(struct cache *l1, struct cache *l2, int l2_transfer_time, int l2_bus_width);

