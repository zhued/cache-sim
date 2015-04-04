/* Cache implementation */

#include "cache.h"
void init_l1_cache(struct l1_config *l1, struct cache *cache)
{
	cache->buf = malloc(l1->l1_cache_size * l1->l1_assoc);
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

void init_l2_cache(struct l2_config *l2, struct cache *cache)
{
	cache->buf = malloc(l2->l2_cache_size * l2->l2_assoc);
}
