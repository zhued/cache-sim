/* Cache Data Structure */

#pragma once

#include <stdbool.h>

struct lru {
	int elem;
	struct lru *next;
};

struct block {
	unsigned long tag;
	int valid;
	int dirty;
};

struct cache {
	int block_size;
	int cache_size;
	int assoc;
	int hit_time;
	int miss_time;
	int req_size;
	int transfer_time;
	int bus_width;
	struct {
		unsigned long requests;
		unsigned long hits;
		unsigned long kickouts;
		unsigned long dirty_kickouts;
		unsigned long flush_kickouts;
		unsigned long reads;
		unsigned long writes;
	} cache_stats;
	struct block *buf;	/* Indexed by cache index then way */
	struct lru **lrus;
	struct cache *backend;
	const char *name;
};


struct stat_struct {
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
};

extern struct cache l1_d;
extern struct cache l1_i;
extern struct cache l2;

extern struct stat_struct stats;

void init_cache(struct cache *cache);

int dispatch_write(struct cache *cache, unsigned long addr, int bytes);
int dispatch_read(struct cache *cache, unsigned long addr, int bytes);
int cache_write(struct cache *cache, unsigned long addr);
int cache_read(struct cache *cache, unsigned long addr);
void l2_l1_transfer(struct cache *l1, struct cache *l2, int l2_transfer_time, int l2_bus_width);
void print_cache(struct cache *cache);
unsigned long cache_flush(struct cache *cache);

static inline unsigned long int log_2(unsigned long int x)
{
	unsigned long int i = 0;
	while (x != 1) { x >>= 1; i++; }
	return i;
}
