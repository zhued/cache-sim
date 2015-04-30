/* Cache implementation */

#include "cache.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

static inline int buf_index(struct cache *cache, int row, int column)
{
	return row * cache->assoc + column;
}

static void print_lru(struct lru *lru)
{
	while (lru) {
		printf("%d", lru->elem);
		if (lru->next) printf(" -> ");
		lru = lru->next;
	}
	printf("\n");
}

struct cache l1_i = {
	.block_size = 32,
	.cache_size = 8196,
	.assoc = 1,
	.hit_time = 1,
	.miss_time = 1,
	.buf = NULL,
	.req_size = 4,
	.name = "l1_i"
};

struct cache l1_d = {
	.block_size = 32,
	.cache_size = 8196,
	.assoc = 1,
	.hit_time = 1,
	.miss_time = 1,
	.buf = NULL,
	.req_size = 4,
	.name = "l1_d"
};

struct cache l2 = {
	.block_size = 64,
	.cache_size = 32768,
	.assoc = 1,
	.hit_time = 5,
	.miss_time = 7,
	.buf = NULL,
	.req_size = 64,
	.name = "l2"
};

struct stat_struct stats;

static void update_lru(struct cache *cache, int index, int way)
{
	struct lru *cur = cache->lrus[index];

	if (cache->assoc <= 1) return;

	if (cur->elem == way) return;

	while (cur->next && cur->next->elem != way) {
		cur = cur->next;
	}

	struct lru *prev = cache->lrus[index];
	cache->lrus[index] = cur->next;
	cur->next = cur->next->next;
	cache->lrus[index]->next = prev;
}

static void decompose_addr(struct cache* cache,
			   unsigned long long addr,
			   unsigned long long *tag,
			   unsigned long long *index,
			   unsigned long long *bi)
{
	*bi = addr & (cache->block_size - 1);
	*index = (addr >> cache->block_bits) & (cache->cache_size - 1);
	*tag = (addr >> cache->block_index_bits);
}

static unsigned long long compose_addr(struct cache* cache,
				       unsigned long long tag,
				       unsigned long long index,
				       unsigned long long bi)
{
	return (tag << cache->block_index_bits) | (index << cache->block_bits);
}

void init_cache(struct cache *cache)
{
	cache->cache_size /= cache->block_size;
	cache->cache_size /= cache->assoc;
	cache->block_bits = log_2(cache->block_size);
	cache->block_index_bits = log_2(cache->block_size * cache->cache_size);
	cache->buf = malloc(cache->cache_size * cache->assoc * sizeof(struct block));
	cache->lrus = malloc(cache->cache_size * sizeof(struct lru*));

	for (int i = 0; i < cache->cache_size; i++) {
		cache->lrus[i] = malloc(sizeof(struct lru));
		struct lru *cur = cache->lrus[i];
		cur->elem = 0;
		for (int j = 1; j < cache->assoc; j++) {
			cur->next = malloc(sizeof(struct lru));
			cur = cur->next;
			cur->elem = j;
		}
		cur->next = NULL;
		for (int j = 0; j < cache->assoc; j++) {
			struct block *b = &cache->buf[buf_index(cache, i, j)];
			b->tag = 0;
			b->valid = 0;
			b->dirty = 0;
		}
	}
}


int dispatch_write(struct cache *cache, unsigned long long addr, int bytes)
{
	unsigned long long block_index, index, tag;
	int cost = 0;
	unsigned long long aligned = addr & ~(cache->req_size - 1);
	decompose_addr(cache, addr, &tag, &index, &block_index);

	bytes -= cache->req_size - (addr - aligned);
	cost += cache_write(cache, aligned);

	while (bytes > 0) {
		aligned += cache->req_size;
		decompose_addr(cache, aligned, &tag, &index, &block_index);
		cost += cache_write(cache, aligned);
		bytes -= cache->req_size;
	}
	return cost;
}

int dispatch_read(struct cache *cache, unsigned long long addr, int bytes)
{
	unsigned long long block_index, index, tag;
	int cost = 0;
	unsigned long long aligned = addr & ~(cache->req_size-1);
	decompose_addr(cache, addr, &tag, &index, &block_index);

	bytes -= cache->req_size - (addr - aligned);
	cost += cache_read(cache, aligned);

	while (bytes > 0) {
		aligned += cache->req_size;
		decompose_addr(cache, aligned, &tag, &index, &block_index);
		cost += cache_read(cache, aligned);
		bytes -= cache->req_size;
	}
	return cost;
}

int cache_write(struct cache *cache, unsigned long long addr)
{
	unsigned long long index, tag, bi;
	decompose_addr(cache, addr, &tag, &index, &bi);

	int row = index * cache->assoc;

	cache->cache_stats.requests++;
	cache->cache_stats.writes++;

	for (int i = 0; i < cache->assoc; i++) {
		int idx = buf_index(cache, index, i);
		if (cache->buf[idx].valid && cache->buf[idx].tag == tag) {
			/* Hit */
			cache->cache_stats.hits++;
			cache->buf[idx].dirty = 1;
			update_lru(cache, index, i);
			return cache->hit_time;
		}
	}

	struct lru *lru = cache->lrus[index];

	while (lru->next) {
		lru = lru->next;
	}
	row = index * cache->assoc + lru->elem;
	update_lru(cache, index, lru->elem);

	/* Miss */
	int cumulative_miss_time = cache->miss_time + cache->hit_time;
	if (cache->buf[row].valid) {
		cache->cache_stats.kickouts++;
		if (cache->buf[row].dirty) {
			cache->cache_stats.dirty_kickouts++;
		}
	}

	if (cache->buf[row].dirty && cache->buf[row].valid) {
		if (cache->backend) {
			unsigned long long writeaddr = compose_addr(cache,
								    cache->buf[row].tag,
								    index,
								    bi);
			cumulative_miss_time += cache_write(cache->backend, writeaddr);
			cumulative_miss_time += cache->transfer_time * (cache->block_size / cache->bus_width);
		} else {
			/* Go to memory */
			cumulative_miss_time += mem.mem_sendaddr + mem.mem_ready +
				(mem.mem_chunktime * cache->block_size / mem.mem_chunksize);
		}
	}

	if (cache->backend) {
		cumulative_miss_time+=cache_read(cache->backend, addr);
		cumulative_miss_time += cache->transfer_time * (cache->block_size / cache->bus_width);
	} else {
		/* Go to memory */
		cumulative_miss_time += mem.mem_sendaddr + mem.mem_ready +
			(mem.mem_chunktime * cache->block_size / mem.mem_chunksize);
	}

	cache->buf[row].tag = tag;
	cache->buf[row].valid = 1;
	cache->buf[row].dirty = 1;

	return cumulative_miss_time;
}

int cache_read(struct cache *cache, unsigned long long addr)
{
	unsigned long long index, tag, bi;
	decompose_addr(cache, addr, &tag, &index, &bi);

	cache->cache_stats.requests++;
	cache->cache_stats.reads++;

	int row = index * cache->assoc;

	for (int i = 0; i < cache->assoc; i++) {
		int idx = buf_index(cache, index, i);
		if (cache->buf[idx].valid && cache->buf[idx].tag == tag) {
			/* Hit */
			cache->cache_stats.hits++;
			update_lru(cache, index, i);
			return cache->hit_time;
		}
	}
	/* Miss */
	int cumulative_miss_time = cache->miss_time + cache->hit_time;
	struct lru *lru = cache->lrus[index];

	while (lru->next) {
		lru = lru->next;
	}

	row = index * cache->assoc + lru->elem;
	update_lru(cache, index, lru->elem);

	if (cache->buf[row].valid) {
		cache->cache_stats.kickouts++;
		if (cache->buf[row].dirty) {
			cache->cache_stats.dirty_kickouts++;
		}
	}

	if (cache->buf[row].dirty && cache->buf[row].valid) {
		if (cache->backend) {
			unsigned long long writeaddr = compose_addr(cache,
							       cache->buf[row].tag,
							       index,
							       bi);
			cumulative_miss_time += cache_write(cache->backend, writeaddr);
			cumulative_miss_time += cache->transfer_time *
				(cache->block_size / cache->bus_width);
		} else {
			/* Go to memory */
			cumulative_miss_time += mem.mem_sendaddr + mem.mem_ready +
				(mem.mem_chunktime * cache->block_size / mem.mem_chunksize);
		}
	}

	if (cache->backend) {
		cumulative_miss_time += cache_read(cache->backend, addr);
		cumulative_miss_time += cache->transfer_time * (cache->block_size / cache->bus_width);
	} else {
		/* Go to memory */
		cumulative_miss_time += mem.mem_sendaddr + mem.mem_ready +
			(mem.mem_chunktime * cache->block_size / mem.mem_chunksize);
	}

	cache->buf[row].tag = tag;
	cache->buf[row].valid = 1;
	cache->buf[row].dirty = 0;

	return cumulative_miss_time;
}

unsigned long long cache_flush(struct cache *cache) {
	unsigned long long cost = 0;
	for (unsigned long long i = 0; i < cache->cache_size; i++) {
		for (int j = 0; j < cache->assoc; j++) {
			struct block *b = &cache->buf[buf_index(cache, i, j)];
			if (b->dirty) {
				if (cache->backend) {
					unsigned long long writeaddr = compose_addr(cache, b->tag, i, 0);
					cost += cache_write(cache->backend, writeaddr);
					cost += cache->transfer_time *
						(cache->block_size / cache->bus_width);
				} else {
					cost += mem.mem_sendaddr + mem.mem_ready +
						(mem.mem_chunktime *
						 cache->block_size / mem.mem_chunksize);
				}
				cache->cache_stats.flush_kickouts++;
			}
			b->valid = 0;
		}
	}
	return cost;
}

void print_cache(struct cache *cache)
{
	for (unsigned long long i = 0; i < cache->cache_size; i++) {
		for (int j = 0; j < cache->assoc; j++) {
			struct block *b = &cache->buf[buf_index(cache, i, j)];
			if (b->valid) {
				printf("Index: %#llx | V:%d D: %d Tag: %#llx\n",
				       i, b->valid, b->dirty, b->tag);
			}
		}
	}
}
