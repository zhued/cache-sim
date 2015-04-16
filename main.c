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
	unsigned long insts = 0;
	while (scanf("%c %lx %d\n", &op, &address, &bytesize) == 3) {
		switch (op) {
		case 'I':
			/* printf("%c %lx %d\n", op, address, bytesize); */
			insts++;
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
		if (insts > 380000) {
			insts = 0;
			cache_flush(l1i);
			cache_flush(l1d);
			cache_flush(l2);
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

	print_cache(&l1_d);

	printf("L1i requests: %ld\n", l1_i.cache_stats.requests);
	printf("L1i hits: %ld\n", l1_i.cache_stats.hits);
	printf("L1i misses: %ld\n", l1_i.cache_stats.requests - l1_i.cache_stats.hits);
	printf("L1i kickout: %ld\n", l1_i.cache_stats.kickouts);
	printf("L1i dirty kickout: %ld\n", l1_i.cache_stats.dirty_kickouts);
	printf("L1i flush kickout: %ld\n\n", l1_i.cache_stats.flush_kickouts);

	printf("L1d requests: %ld\n", l1_d.cache_stats.requests);
	printf("L1d hits: %ld\n", l1_d.cache_stats.hits);
	printf("L1d misses: %ld\n", l1_d.cache_stats.requests - l1_d.cache_stats.hits);
	printf("L1d kickout: %ld\n", l1_d.cache_stats.kickouts);
	printf("L1d dirty kickout: %ld\n", l1_d.cache_stats.dirty_kickouts);
	printf("L1d flush kickout: %ld\n\n", l1_d.cache_stats.flush_kickouts);

	printf("L2 requests: %ld\n", l2.cache_stats.requests);
	printf("L2 hits: %ld\n", l2.cache_stats.hits);
	printf("L2 misses: %ld\n", l2.cache_stats.requests - l2.cache_stats.hits);
	printf("L2 kickout: %ld\n", l2.cache_stats.kickouts);
	printf("L2 dirty kickout: %ld\n\n", l2.cache_stats.dirty_kickouts);

	return 0;
}
