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

// void output_stats(struct stats *result, struct mem_config *memm struct cache *l1i, struct cache *l1d, struct cache *l2)
// {
// 	printf("------------------------------------------------------------\n");
// 	printf(" 		<trace>.<config>		Similulation Results          ")
// 	printf("------------------------------------------------------------\n");
// 	printf("\n");
// 	printf("Memory System:\n");
// 	printf("    Dcache Size = %d : ways = %d : block size = %d\n", l1i.cache_size, l1i.assoc,l1i.block_size);
// 	printf("    Icache Size = %d : ways = %d : block size = %d\n", l1d.cache_size, l1d.assoc,l1d.block_size);
// 	printf("    L2-cache Size = %d : ways = %d : block size = %d\n", l2.cache_size, l2.assoc,l2.block_size);
// 	printf("    Memory ready  time = %d : chunksize = %d : chunktime = %d\n", <memready>,<mchunks>,<mchunkt>);
// 	printf("\n");
	
// 	printf("Execute Time = %lld; Total References = %lld\n", <extime>, <refnum+1>);
// 	printf("Flush Time = %lld\n", <flushtime>);
// 	printf("Inst refs = %lld; Data refs = %lld\n",result.reads,result.write_cycles+result.read_cycles);
// 	printf("\n");
	
// 	printf("Number of references types: [Percentage]\n");
// 	printf("    Reads  =     %lld    [%f%%]\n", result.reads ,100*((double)result.reads)/(result.reads+result.writes+result.insts));
// 	printf("    Writes =     %lld    [%f%%]\n", result.writes,100*((double)result.writes)/(result.reads+result.writes+result.insts));
// 	printf("    Inst.  =     %lld    [%f%%]\n", result.insts,100*((double)result.insts)/(result.reads+result.writes+result.insts));
// 	printf("    Total  =     %lld\n",result.reads+result.writes+result.insts);
// 	printf("\n");
	
// 	printf("Total cycles for activities: [Percentage]\n");
// 	printf("    Reads  =     %lld    [%f%%]\n",dcache->rtime+l2cache->rtime,100*((double)(dcache->rtime+l2cache->rtime))/extime);
// 	printf("    Writes =     %lld    [%f%%]\n",dcache->wtime+l2cache->wtime,100*((double)(dcache->wtime+l2cache->wtime))/extime);
// 	printf("    Inst.  =     %lld    [%f%%]\n",icache->itime+l2cache->itime,100*((double)(icache->itime+l2cache->itime))/extime);
// 	printf("    Total  =     %lld\n",extime);
// 	printf("\n");
	
// 	printf("Average cycles for activiites: [Percentage]\n");
// 	printf("  Read =  %f; Write =  %f; Inst. =  %f\n",((double)dcache->rtime+l2cache->rtime)/(dcache->rrefs),((double)dcache->wtime+l2cache->wtime)/(dcache->wrefs),((double)extime)/(icache->rrefs));
// 	printf("Ideal: Exec. Time = %lld; CPI =  %f\n",dcache->rrefs+dcache->wrefs+2*icache->rrefs,((double)dcache->rrefs+dcache->wrefs+2*icache->rrefs)/(icache->rrefs));
// 	printf("Ideal mis-aligned: Exec. Time = %lld; CPI =  %f\n",dcache->rrefs+dcache->wrefs+2*icache->rrefs+misallignment,((double)dcache->rrefs+dcache->wrefs+2*icache->rrefs+misallignment)/(icache->rrefs));
// 	printf("\n");

// 	printf("Memory Level: L1i\n");
// 	printf("    Hit Count = %lld  Miss Count = %lld\n",icache->hits,icache->misses);
// 	printf("    Total Requests = %lld\n",icache->hits+icache->misses);
// 	printf("    Hit Rate = %f%%   Miss Rate = %f%%\n",100*((float)(icache->hits)/(icache->hits+icache->misses)),100*((float)(icache->misses)/(icache->misses+icache->hits)));
// 	printf("    Kickouts = %lld; Dirty Kickouts = %lld; Transfers = %lld\n",icache->ko,icache->dko,icache->misses);
// 	printf("\n");
	
// 	printf("Memory Level: L1d\n");
// 	printf("    Hit Count = %lld  Miss Count = %lld\n",dcache->hits,dcache->misses);
// 	printf("    Total Requests = %lld\n",dcache->hits+dcache->misses);
// 	printf("    Hit Rate = %f%%   Miss Rate = %f%%\n",100*((float)(dcache->hits)/(dcache->hits+dcache->misses)),100*((float)(dcache->misses)/(dcache->misses+dcache->hits)));
// 	printf("    Kickouts = %lld; Dirty Kickouts = %lld; Transfers = %lld\n",dcache->ko,dcache->dko,dcache->misses);
// 	printf("\n");
	
// 	printf("Memory Level: L2\n");
// 	printf("    Hit Count = %lld  Miss Count = %lld\n",l2cache->hits,l2cache->misses);
// 	printf("    Total Requests = %lld\n",l2cache->hits+l2cache->misses);
// 	printf("    Hit Rate = %f%%   Miss Rate = %f%%\n",100*((float)(l2cache->hits)/(l2cache->hits+l2cache->misses)),100*((float)(l2cache->misses)/(l2cache->misses+l2cache->hits)));
// 	printf("    Kickouts = %lld; Dirty Kickouts = %lld; Transfers = %lld\n",l2cache->ko,l2cache->dko,l2cache->misses);
// 	printf("\n");

// 	printf("L1 cache cost (Icache $%d) + (Dcache $%d) = $%d\n", icache->cost, dcache->cost, icache->cost+dcache->cost);
// 	printf("L2 cache cost = $%d;   Memory Cost = $%d\n", l2cache->cost, memcost);
// 	printf("Total cost = $%d\n", memcost+icache->cost+dcache->cost+l2cache->cost);
// 	printf("\n");

// 	return 0;
// }

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
