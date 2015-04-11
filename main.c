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
			printf("%c %lx %d\n", op, address, bytesize);
			dispatch_read(l1i, address, bytesize);
			break;
		case 'W':
			dispatch_write(l1d, address, bytesize);
			break;
		case 'R':
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

	process_trace(&l1_i, &l1_d, &l2, &mem);

	return 0;
}
