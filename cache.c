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

struct cache l1_i = {
	.block_size = 32,
	.cache_size = 8192,
	.assoc = 1,
	.hit_time = 1,
	.miss_time = 1,
	.buf = NULL
};

struct cache l1_d = {
	.block_size = 32,
	.cache_size = 8192,
	.assoc = 1,
	.hit_time = 1,
	.miss_time = 1,
	.buf = NULL
};

struct cache l2 = {
	.block_size = 64,
	.cache_size = 32768,
	.assoc = 1,
	.hit_time = 5,
	.miss_time = 7,
	.buf = NULL
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

static void decompose_addr(struct cache* cache, unsigned long addr, int *tag, int *index, int *bi)
{
	int block_bits = log_2(cache->block_size);
	int block_index_bits = log_2(cache->block_size * cache->cache_size);

	*bi = addr & (cache->block_size - 1);
	*index = (addr >> block_bits) & (cache->cache_size - 1);
	*tag = (addr >> block_index_bits) & ( (1 << (48 - block_index_bits)) - 1 );
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
	}
}


void dispatch_write(struct cache *cache, unsigned long addr, int bytes)
{
	int block_index, index, tag;
	decompose_addr(cache, addr, &tag, &index, &block_index);
	
	bytes -= cache->block_size - block_index;
	cache_write(cache, index, tag);

	while (bytes > 0) {
		index++;
		cache_write(cache, index, tag);
		bytes -= cache->block_size;
	}
}

void dispatch_read(struct cache *cache, unsigned long addr, int bytes)
{
	int block_index, index, tag;
	unsigned long aligned = addr & ~(cache->block_size - 1);
	decompose_addr(cache, addr, &tag, &index, &block_index);

	bytes -= cache->block_size - block_index;
	cache_read(cache, index, tag);

	while (bytes > 0) {
		aligned += cache->block_size;
		decompose_addr(cache, aligned, &tag, &index, &block_index);
		cache_read(cache, index, tag);
		bytes -= cache->block_size;
	}
}

bool cache_write(struct cache *cache, unsigned long index, unsigned long tag)
{
	/* int block_index, index, tag; */
	/* decompose_addr(cache, addr, &tag, &index, &block_index); */
	return false;
}

bool cache_read(struct cache *cache, unsigned long index, unsigned long tag)
{
	/* int block_index, index, tag; */
	/* decompose_addr(cache, addr, &tag, &index, &block_index); */

	int row = index * cache->assoc * sizeof(struct block);

	for (int i = 0; i < cache->assoc; i++) {
		int idx = row + i * sizeof(struct block);
		if (cache->buf[idx].valid && cache->buf[idx].tag == tag) {
			/* Hit */
			return false;
		}
	}

	/* struct lru *cur = cache->lrus[index]; */
	/* for (int i = 0; i < cache->assoc - 1; i++) { */
	/* 	cur = cur->next; */
	/* } */

	/* if (cache->buf[row + cur->elem * sizeof(struct block)].dirty) { */
	/* 	if (cache->backend) { */
	/* 		cache_write(cache->backend, addr, bytes); */
	/* } */

	/* cache->buf[row + cur->elem * sizeof(struct block)].tag = tag; */
	/* cache->buf[row + cur->elem * sizeof(struct block)].valid = 1; */
	/* cache->buf[row + cur->elem * sizeof(sizeof block)].dirty = 0; */

	printf("Index: %lx\n", index);
	printf("Tag: %lx\n", tag);
	return true;
}
