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

struct cache_stats {
	unsigned long requests;
	unsigned long hits;
	unsigned long kickouts;
	unsigned long dirty_kickouts;
	unsigned long transfers;
	unsigned long flush_kickouts;
}

struct stats {
	unsigned long reads;
	unsigned long writes;
	unsigned long insts;
	unsigned long read_cycles;
	unsigned long write_cycles;
	unsigned long inst_cycles;
	unsigned long flush_time;
	unsigned long ideal_cycles;
	unsigned long ideal_misaligned;
	unsigned long flushes;
	unsigned long invalidates;
	struct cache_stats l1i;
	struct cache_stats l1d;
	struct cache_stats l2;
};

extern struct cache l1_d;
extern struct cache l1_i;
extern struct cache l2;

void init_cache(struct cache *cache);

void cache_write(struct cache *cache);
void cache_read(struct cache *cache);
void l2_l1_transfer(struct cache *l1, struct cache *l2, int l2_transfer_time, int l2_bus_width);
