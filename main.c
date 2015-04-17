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
		/* printf("%c %lx %d\n", op, address, bytesize); */
		switch (op) {
		case 'I':
			insts++;
			dispatch_read(l1i, address, bytesize);
			stats.insts++;
			break;
		case 'W':
			dispatch_write(l1d, address, bytesize);
			stats.writes++;
			break;
		case 'R':
			dispatch_read(l1d , address, bytesize);
			stats.reads++;
			break;
		}
		if (insts > 380000) {
			printf("Cache flush\n");
			insts = 0;
			cache_flush(l1i);
			cache_flush(l1d);
			cache_flush(l2);
		}
	}
}

void output_stats(struct stat_struct *result, struct mem_config *mem, struct cache *l1_i, struct cache *l1_d, struct cache *l2)
{
	printf("------------------------------------------------------------\n");
	printf(" 		<trace>.<config>		Similulation Results          \n");
	printf("------------------------------------------------------------\n");
	printf("\n");
	printf("Memory System:\n");
	printf("    Dcache Size = %d : ways = %d : block size = %d\n", l1_d->cache_size * l1_d->block_size, l1_d->assoc,l1_d->block_size);
	printf("    Icache Size = %d : ways = %d : block size = %d\n", l1_i->cache_size * l1_i->block_size, l1_i->assoc,l1_i->block_size);
	printf("    L2-cache Size = %d : ways = %d : block size = %d\n", l2->cache_size * l2->block_size, l2->assoc,l2->block_size);
	// printf("    Memory ready  time = %d : chunksize = %d : chunktime = %d\n", <memready>,<mchunks>,<mchunkt>);
	printf("\n");
	
	// printf("Execute Time = %lld; Total References = %lld\n", <extime>, <refnum+1>);
	// printf("Flush Time = %lld\n", <flushtime>);
	printf("Inst refs = %lld; Data refs = %lld\n",result->insts,result->reads + result->writes);
	printf("\n");
	
	printf("Number of references types: [Percentage]\n");
	printf("    Reads  =     %lld    [%.1f%%]\n", result->reads ,100*((double)result->reads)/(result->reads+result->writes+result->insts));
	printf("    Writes =     %lld    [%.1f%%]\n", result->writes,100*((double)result->writes)/(result->reads+result->writes+result->insts));
	printf("    Inst.  =     %lld    [%.1f%%]\n", result->insts,100*((double)result->insts)/(result->reads+result->writes+result->insts));
	printf("    Total  =     %lld\n",result->reads+result->writes+result->insts);
	printf("\n");

	unsigned long int extime = result->read_cycles + result->write_cycles + result->inst_cycles;
	printf("Total cycles for activities: [Percentage]\n");
	printf("    Reads  =     %lld    [%.1f%%]\n",result->read_cycles,100*((double)(result->read_cycles))/extime);
	printf("    Writes =     %lld    [%.1f%%]\n",result->write_cycles,100*((double)(result->write_cycles))/extime);
	printf("    Inst.  =     %lld    [%.1f%%]\n",result->inst_cycles,100*((double)(result->inst_cycles))/extime);
	printf("    Total  =     %lld\n",extime);
	printf("\n");
	
	/* printf("Average cycles for activiites: [Percentage]\n"); */
	/* printf("  Read =  %f; Write =  %f; Inst. =  %f\n",((double)result->read_cycles)/(dcache->rrefs),((double)result->write_cycles)/(dcache->wrefs),((double)extime)/(icache->rrefs)); */
	/* printf("Ideal: Exec. Time = %lld; CPI =  %f\n",dcache->rrefs+dcache->wrefs+2*icache->rrefs,((double)dcache->rrefs+dcache->wrefs+2*icache->rrefs)/(icache->rrefs)); */
	/* printf("Ideal mis-aligned: Exec. Time = %lld; CPI =  %f\n",dcache->rrefs+dcache->wrefs+2*icache->rrefs+misallignment,((double)dcache->rrefs+dcache->wrefs+2*icache->rrefs+misallignment)/(icache->rrefs)) */;
	printf("\n");

	printf("Memory Level: L1i\n");
	printf("    Hit Count = %lld  Miss Count = %lld\n",l1_i->cache_stats.hits, l1_i->cache_stats.requests - l1_i->cache_stats.hits);
	printf("    Total Requests = %lld\n",l1_i->cache_stats.requests);
	printf("    Hit Rate = %.1f%%   Miss Rate = %.1f%%\n",100*((float)(l1_i->cache_stats.hits)/(l1_i->cache_stats.requests)),100*((float)(l1_i->cache_stats.requests - l1_i->cache_stats.hits)/(l1_i->cache_stats.requests)));
	printf("    Kickouts = %lld; Dirty Kickouts = %lld; Transfers = %lld\n",l1_i->cache_stats.kickouts,l1_i->cache_stats.dirty_kickouts,l1_i->cache_stats.transfers);
	printf("    Flush Kickouts  =  %lld\n", l1_i->cache_stats.flush_kickouts);
	printf("\n");
	
	printf("Memory Level: L1d\n");
	printf("    Hit Count = %ld  Miss Count = %ld\n",l1_d->cache_stats.hits,l1_d->cache_stats.requests - l1_d->cache_stats.hits);
	printf("    Total Requests = %lld\n",l1_d->cache_stats.requests);
	printf("    Hit Rate = %.1f%%   Miss Rate = %.1f%%\n",100*((float)(l1_d->cache_stats.hits)/(l1_d->cache_stats.requests)),100*((float)(l1_d->cache_stats.requests - l1_d->cache_stats.hits)/(l1_d->cache_stats.requests)));
	printf("    Kickouts = %lld; Dirty Kickouts = %lld; Transfers = %lld\n",l1_d->cache_stats.kickouts,l1_d->cache_stats.dirty_kickouts,l1_d->cache_stats.transfers);
	printf("    Flush Kickouts  =  %lld\n", l1_d->cache_stats.flush_kickouts);
	printf("\n");
	
	printf("Memory Level: L2\n");
	printf("    Hit Count = %lld  Miss Count = %lld\n",l2->cache_stats.hits,l2->cache_stats.requests - l2->cache_stats.hits);
	printf("    Total Requests = %lld\n",l2->cache_stats.requests);
	printf("    Hit Rate = %.1f%%   Miss Rate = %.1f%%\n",100*((float)(l2->cache_stats.hits)/(l2->cache_stats.requests)),100*((float)(l2->cache_stats.requests - l2->cache_stats.hits)/(l2->cache_stats.requests)));
	printf("    Kickouts = %lld; Dirty Kickouts = %lld; Transfers = %lld\n",l2->cache_stats.kickouts,l2->cache_stats.dirty_kickouts,l2->cache_stats.transfers);
	printf("    Flush Kickouts  =  %lld\n", l2->cache_stats.flush_kickouts);
	printf("\n");

	int l1_i_cost = ((l1_i->cache_size * l1_i->block_size * l1_i->assoc) / 4096) * (100 + 100 * log_2(l1_i->assoc));
	int l1_d_cost = ((l1_d->cache_size * l1_d->block_size * l1_d->assoc) / 4096) * (100 + 100 * log_2(l1_d->assoc));
	printf("Size of L2: %d\n", (l2->cache_size * l2->block_size * l2->assoc));
	int l2_cost = (50 * (l2->cache_size * l2->block_size * l2->assoc)) / 65536 + 50 * log_2(l2->assoc);
	int memcost = log_2(mem->mem_ready / 30) * 200 + log_2(mem->mem_chunktime / 15) * 100 + 75;
	printf("L1 cache cost (Icache $%d) + (Dcache $%d) = $%d\n", l1_i_cost, l1_d_cost, l1_i_cost+l1_d_cost);
	printf("L2 cache cost = $%d;   Memory Cost = $%d\n", l2_cost, memcost);
	/* printf("Total cost = $%d\n", l1_i_cost + l1_d_cost + l2_cost + memcost); */
	printf("\n");
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

	process_trace(&l1_i, &l1_d, &l2, &mem);

	print_cache(&l1_d);

	/* printf("L1i requests: %ld\n", l1_i.cache_stats.requests); */
	/* printf("L1i hits: %ld\n", l1_i.cache_stats.hits); */
	/* printf("L1i misses: %ld\n", l1_i.cache_stats.requests - l1_i.cache_stats.hits); */
	/* printf("L1i kickout: %ld\n", l1_i.cache_stats.kickouts); */
	/* printf("L1i dirty kickout: %ld\n", l1_i.cache_stats.dirty_kickouts); */
	/* printf("L1i flush kickout: %ld\n\n", l1_i.cache_stats.flush_kickouts); */

	/* printf("L1d requests: %ld\n", l1_d.cache_stats.requests); */
	/* printf("L1d hits: %ld\n", l1_d.cache_stats.hits); */
	/* printf("L1d misses: %ld\n", l1_d.cache_stats.requests - l1_d.cache_stats.hits); */
	/* printf("L1d kickout: %ld\n", l1_d.cache_stats.kickouts); */
	/* printf("L1d dirty kickout: %ld\n", l1_d.cache_stats.dirty_kickouts); */
	/* printf("L1d flush kickout: %ld\n\n", l1_d.cache_stats.flush_kickouts); */

	/* printf("L2 requests: %ld\n", l2.cache_stats.requests); */
	/* printf("L2 hits: %ld\n", l2.cache_stats.hits); */
	/* printf("L2 misses: %ld\n", l2.cache_stats.requests - l2.cache_stats.hits); */
	/* printf("L2 kickout: %ld\n", l2.cache_stats.kickouts); */
	/* printf("L2 dirty kickout: %ld\n\n", l2.cache_stats.dirty_kickouts); */
	
	output_stats(&stats, &mem, &l1_i, &l1_d, &l2);

	return 0;
}
