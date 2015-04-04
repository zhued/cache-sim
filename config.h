/* Configuration structure definition */

#pragma once

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "cache.h"

struct mem_config {
	int mem_sendaddr;
	int mem_ready;
	int mem_chunktime;
	int mem_chunksize;
};

extern struct mem_config mem;

void load_config(char *path,
		 struct cache *l1_i,
		 struct cache *l1_d,
		 struct cache *l2,
		 struct mem_config *mem,
		 int *l2_transfer,
		 int *l2_bus_width);
