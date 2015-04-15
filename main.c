/* Main */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "config.h"
#include "cache.h"

void process_trace(struct cache *l1i,
		   struct cache *l1d,
		   struct cache *l2,
		   struct mem_config *mem)
{
	char op;
	unsigned long address;
	int bytesize;
	while (scanf("%c %lx %d\n", &op, &address, &bytesize) == 3) {
		switch (op) {
		case 'I':
			/* printf("%c %lx %d\n", op, address, bytesize); */
			dispatch_read(l1i, address, bytesize);
			break;
		case 'W':
			/* printf("%c %lx %d\n", op, address, bytesize); */
			dispatch_write(l1d, address, bytesize);
			break;
		case 'R':
			/* printf("%c %lx %d\n", op, address, bytesize); */
			dispatch_read(l1d , address, bytesize);
			break;
		}
	}
}

int main(int argc, char **argv)
{
	int transfer_time;
	int bus_width;

	/* Load configuration from argv[1] if it exists */
	if (argc > 1) {
		load_config(argv[1], &l1_i, &l1_d, &l2, &mem, &transfer_time, &bus_width);
	}

	init_cache(&l1_i);
	init_cache(&l1_d);
	init_cache(&l2);

	l1_i.backend = &l2;
	l1_d.backend = &l2;
	l2.backend = NULL;

	l1_i.cache_stats.requests=0;
	l1_d.cache_stats.requests=0;
	l2.cache_stats.requests=0;

	process_trace(&l1_i, &l1_d, &l2, &mem);

	print_cache(&l1_i);

	printf("L1i requests: %d\n", l1_i.cache_stats.requests);
	printf("L1i hits: %d\n", l1_i.cache_stats.hits);
	printf("L1i misses: %d\n\n", l1_i.cache_stats.requests - l1_i.cache_stats.hits);

	printf("L1d requests: %d\n", l1_d.cache_stats.requests);
	printf("L1d hits: %d\n", l1_d.cache_stats.hits);
	printf("L1d misses: %d\n\n", l1_d.cache_stats.requests - l1_d.cache_stats.hits);

	printf("L2 requests: %d\n", l2.cache_stats.requests);
	printf("L2 hits: %d\n", l2.cache_stats.hits);
	printf("L2 misses: %d\n\n", l2.cache_stats.requests - l2.cache_stats.hits);

	return 0;
}
