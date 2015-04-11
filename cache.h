/* Cache Data Structure */

#pragma once

#include <stdbool.h>

struct lru {
	int elem;
	struct lru *next;
};

struct block {
	int tag;
	int valid;
	int dirty;
};

struct cache {
	int block_size;
	int cache_size;
	int assoc;
	int hit_time;
	int miss_time;
	struct {
		unsigned long requests;
		unsigned long hits;
		unsigned long kickouts;
		unsigned long dirty_kickouts;
		unsigned long transfers;
		unsigned long flush_kickouts;
	} cache_stats;
	struct block *buf;	/* Indexed by cache index then way */
	struct lru **lrus;
	struct cache *backend;
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

void dispatch_write(struct cache *cache, unsigned long addr, int bytes);
void dispatch_read(struct cache *cache, unsigned long addr, int bytes);
bool cache_write(struct cache *cache, unsigned long index, unsigned long tag);
bool cache_read(struct cache *cache, unsigned long index, unsigned long tag);
void l2_l1_transfer(struct cache *l1, struct cache *l2, int l2_transfer_time, int l2_bus_width);
