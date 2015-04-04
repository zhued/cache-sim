/* Main */

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "cache.h"

void process_trace(struct cache *l1i,
		   struct cache *l1d,
		   struct cache *l2,
		   struct mem_config *mem,
		   struct stats *st)
{
	char op;
	unsigned long long address;
	unsigned int bytesize;
	while (scanf("%c %Lx %d\n", &op, &address, &bytesize) == 3) {
		printf("%c %Lx %d\n", op, address, bytesize);
	}
}

int main(int argc, char **argv)
{
	int transfer_time;
	int bus_width;

	struct stats sim_stats;
	/* Load configuration from argv[1] if it exists */

	if (argc > 1) {
		load_config(argv[1], &l1_i, &l1_d, &l2, &mem, &transfer_time, &bus_width);
	}

	process_trace(&l1_i, &l1_d, &l2, &mem, &sim_stats);

	return 0;
}
