/* Configuration structure definition */

#pragma once

void load_config(char *path, struct l1_config *l1, struct l2_config *l2);
struct mem_config {
	int mem_sendaddr;
	int mem_ready;
	int mem_chunktime;
	int mem_chunksize;
};

extern struct mem_config mem;
