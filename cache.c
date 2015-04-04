/* Cache implementation */

#include "cache.h"
#include <stdlib.h>

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

void init_cache(struct cache *cache)
{
	cache->buf = malloc(cache->cache_size * cache->assoc);
}
