/* Main */

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "cache.h"

int main(int argc, char **argv)
{
	int transfer_time;
	int bus_width;
	/* Load configuration from argv[1] if it exists */

	if (argc > 1) {
		load_config(argv[1], &l1_i, &l1_d, &l2, &mem, &transfer_time, &bus_width);
	}

	return 0;
}
