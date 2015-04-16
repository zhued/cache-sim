/* Cache implementation */

#include "cache.h"
#include <stdlib.h>
#include <stdio.h>

static inline unsigned long int log_2(unsigned long int x)
{
	unsigned long int i = 0;
	while (x != 1) { x >>= 1; i++; }
	return i;
}

static inline int buf_index(struct cache *cache, int row, int column)
{
	return row * cache->assoc + column;
}

struct cache l1_i = {
	.block_size = 32,
	.cache_size = 256,
	.assoc = 1,
	.hit_time = 1,
	.miss_time = 1,
	.buf = NULL,
	.req_size = 4,
	.name = "l1_i"
};

struct cache l1_d = {
	.block_size = 32,
	.cache_size = 256,
	.assoc = 1,
	.hit_time = 1,
	.miss_time = 1,
	.buf = NULL,
	.req_size = 4,
	.name = "l1_d"
};

struct cache l2 = {
	.block_size = 64,
	.cache_size = 512,
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

	if (way == 0) return;

	for (int i = 0; i < way - 1; i++) {
		cur = cur->next;
	}

	struct lru *prev = cache->lrus[index];
	cache->lrus[index] = cur->next;
	cur->next = cur->next->next;
	cache->lrus[index]->next = prev;
}

static void decompose_addr(struct cache* cache,
			   unsigned long addr,
			   unsigned long *tag,
			   unsigned long *index,
			   unsigned long *bi)
{
	unsigned long block_bits = log_2(cache->block_size);
	unsigned long block_index_bits = log_2(cache->block_size * cache->cache_size);

	*bi = addr & (cache->block_size - 1);
	*index = (addr >> block_bits) & (cache->cache_size - 1);
	*tag = (addr >> block_index_bits);
}

static unsigned long compose_addr(struct cache* cache,
				  unsigned long tag,
				  unsigned long index,
				  unsigned long bi)
{
	unsigned long block_bits = log_2(cache->block_size);
	unsigned long block_index_bits = log_2(cache->block_size * cache->cache_size);

	return (tag << block_index_bits) | (index << block_bits);
}

void init_cache(struct cache *cache)
{
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
			/* printf("%#lx\n", b); */
			b->tag = 0;
			b->valid = 0;
			b->dirty = 0;
		}
	}
}


void dispatch_write(struct cache *cache, unsigned long addr, int bytes)
{
	unsigned long block_index, index, tag;
	unsigned long aligned = addr & ~(cache->req_size - 1);
	decompose_addr(cache, addr, &tag, &index, &block_index);

	bytes -= cache->req_size - (addr - aligned);
	cache_write(cache, aligned);

	while (bytes > 0) {
		aligned += cache->req_size;
		decompose_addr(cache, aligned, &tag, &index, &block_index);
		cache_write(cache, aligned);
		bytes -= cache->req_size;
	}
}

void dispatch_read(struct cache *cache, unsigned long addr, int bytes)
{
	unsigned long block_index, index, tag;
	unsigned long aligned = addr & ~(cache->req_size-1);
	decompose_addr(cache, addr, &tag, &index, &block_index);

	bytes -= cache->req_size - (addr - aligned);
	cache_read(cache, aligned);

	while (bytes > 0) {
		/* if (strcmp(cache->name, "l2") == 0) */
		/* 	printf("L2: %d\n", bytes); */
		aligned += cache->req_size;
		decompose_addr(cache, aligned, &tag, &index, &block_index);
		cache_read(cache, aligned);
		bytes -= cache->req_size;
	}
}

bool cache_write(struct cache *cache, unsigned long addr)
{
	unsigned long index, tag, bi;
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
			return false;
		}
	}

	/* Miss */
	if (cache->buf[row].valid) {
		cache->cache_stats.kickouts++;
		if (cache->buf[row].dirty) {
			cache->cache_stats.dirty_kickouts++;
		}
	}

	if (cache->buf[row].dirty && cache->buf[row].valid) {
		if (cache->backend) {
			unsigned long writeaddr = compose_addr(cache,
							       cache->buf[row].tag,
							       index,
							       bi);
			/* dispatch_write(cache->backend, writeaddr, cache->block_size); */
			cache_write(cache->backend, writeaddr);
		} else {
			/* Go to memory */
		}
	}

	if (cache->backend) {
		/* dispatch_read(cache->backend, addr, cache->block_size); */
		cache_read(cache->backend, addr);
	} else {
		/* Go to memory */
	}

	cache->buf[row].tag = tag;
	cache->buf[row].valid = 1;
	cache->buf[row].dirty = 1;

	return true;
}

bool cache_read(struct cache *cache, unsigned long addr)
{
	unsigned long index, tag, bi;
	decompose_addr(cache, addr, &tag, &index, &bi);

	int row = index * cache->assoc;

	cache->cache_stats.requests++;
	cache->cache_stats.reads++;

	for (int i = 0; i < cache->assoc; i++) {
		int idx = buf_index(cache, index, i);
		if (cache->buf[idx].valid && cache->buf[idx].tag == tag) {
			/* Hit */
			cache->cache_stats.hits++;
			return false;
		}
	}

	/* Miss */
	if (cache->buf[row].valid) {
		cache->cache_stats.kickouts++;
		if (cache->buf[row].dirty) {
			cache->cache_stats.dirty_kickouts++;
		}
	}

	if (cache->buf[row].dirty && cache->buf[row].valid) {
		if (cache->backend) {
			unsigned long writeaddr = compose_addr(cache,
							       cache->buf[row].tag,
							       index,
							       bi);
			/* dispatch_write(cache->backend, writeaddr, cache->block_size); */
			cache_write(cache->backend, writeaddr);
		} else {
			/* Go to memory */
		}
	}

	if (cache->backend) {
		/* dispatch_read(cache->backend, addr & ~(cache->block_size-1), cache->block_size); */
		cache_read(cache->backend, addr);
	} else {
		/* Go to memory */
	}

	cache->buf[row].tag = tag;
	cache->buf[row].valid = 1;
	cache->buf[row].dirty = 0;

	return true;
}

void cache_flush(struct cache *cache) {
	for (unsigned long i = 0; i < cache->cache_size; i++) {
		struct block *b = &cache->buf[buf_index(cache, i, 0)];
		if (b->dirty) {
			if (cache->backend) {
				int writeaddr = compose_addr(cache, b->tag, i, 0);
				dispatch_read(cache->backend, writeaddr, cache->block_size);
			}
			cache->cache_stats.flush_kickouts++;
		}
		b->valid = 0;
	}
}

void print_cache(struct cache *cache)
{
	for (unsigned long i = 0; i < cache->cache_size; i++) {
		struct block *b = &cache->buf[buf_index(cache, i, 0)];
		if (b->valid) {
			printf("Index: %#lx | V:%d D: %d Tag: %#lx\n", i, b->valid, b->dirty, b->tag);
		}
	}
}
