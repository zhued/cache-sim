/* Configuration Structure */

#include "config.h"

struct mem_config mem = {
	.mem_sendaddr = 10,
	.mem_ready = 30,
	.mem_chunktime = 15,
	.mem_chunksize = 8
};

void load_config(char *path, struct cache *l1, struct cache *l2, int *l2_)
{
	return;
}
