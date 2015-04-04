/* Configuration Structure */

#include "config.h"

struct mem_config mem = {
	.mem_sendaddr = 10,
	.mem_ready = 30,
	.mem_chunktime = 15,
	.mem_chunksize = 8
};

void load_config(char *path,
		 struct cache *l1_i,
		 struct cache *l1_d,
		 struct cache *l2,
		 struct mem_config *mem,
		 int *l2_transfer,
		 int *l2_bus_width)
{
	FILE *file = fopen(path, "r");
	if (file == NULL) return;

	while (!feof(file)) {
		char *field = NULL;
		char c;
		size_t size = 0;
		int field_val;
		getdelim(&field, &size, '=', file);
		field[strlen(field) - 1] = '\0';
		while (isspace(c = fgetc(file)));
		ungetc(c, file);
		fscanf(file, "%d\n", &field_val);

		if (strcmp(field, "l1_block_size_i") == 0) l1_i->block_size = field_val;
		else if (strcmp(field, "l1_cache_size_i") == 0) l1_i->cache_size = field_val;
		else if (strcmp(field, "l1_assoc_i") == 0) l1_i->assoc = field_val;
		else if (strcmp(field, "l1_hit_time_i") == 0) l1_i->hit_time = field_val;
		else if (strcmp(field, "l1_miss_time_i") == 0) l1_i->miss_time = field_val;

		else if (strcmp(field, "l1_block_size_d") == 0) l1_d->block_size = field_val;
		else if (strcmp(field, "l1_cache_size_d") == 0) l1_d->cache_size = field_val;
		else if (strcmp(field, "l1_assoc_d") == 0) l1_d->assoc = field_val;
		else if (strcmp(field, "l1_hit_time_d") == 0) l1_d->hit_time = field_val;
		else if (strcmp(field, "l1_miss_time_d") == 0) l1_d->miss_time = field_val;

		else if (strcmp(field, "l2_block_size") == 0) l2->block_size = field_val;
		else if (strcmp(field, "l2_cache_size") == 0) l2->cache_size = field_val;
		else if (strcmp(field, "l2_assoc") == 0) l2->assoc = field_val;
		else if (strcmp(field, "l2_hit_time") == 0) l2->hit_time = field_val;
		else if (strcmp(field, "l2_miss_time") == 0) l2->miss_time = field_val;

		else if (strcmp(field, "l2_transfer") == 0) *l2_transfer = field_val;
		else if (strcmp(field, "l2_bus_width") == 0) *l2_bus_width = field_val;

		else if (strcmp(field, "mem_sendaddr") == 0) mem->mem_sendaddr = field_val;
		else if (strcmp(field, "mem_ready") == 0) mem->mem_ready = field_val;
		else if (strcmp(field, "mem_chunktime") == 0) mem->mem_chunktime = field_val;
		else if (strcmp(field, "mem_chunksize") == 0) mem->mem_chunksize = field_val;
		free(field);
	}

	return;
}
