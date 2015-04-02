/* Main */

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

void load_config(char *path, struct cache *l1, struct cache *l2, 

int main(int argc, char **argv)
{
	struct l1_config l1conf;
	struct l2_config l2conf;
	/* Load configuration from argv[1] if it exists */

	if (argc > 1) {
		load_config(argv[1], &l1conf, &l2conf);
	} else {
		l1conf = l1_default;
		l2conf = l2_default;
	}
	
	return 0;
}
