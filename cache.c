/* Cache implementation */

void init_l1_cache(struct l1_config *l1, struct cache *cache)
{
	cache->buf = malloc(l1->l1_cache_size * l1->l1_assoc);
}

void init_l2_cache(struct l2_config *l2, struct cache *cache)
{
	cache->buf = malloc(l2->l2_cache_size * l2->l2_assoc);
}
