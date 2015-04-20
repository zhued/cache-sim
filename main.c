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
	int cost;
	while (scanf("%c %lx %d\n", &op, &address, &bytesize) == 3) {
		switch (op) {
		case 'I':
			insts++;
			cost = dispatch_read(l1i, address, bytesize);
			stats.inst_cycles += cost;
			stats.insts++;
			if (insts > 380000) {
				insts = 0;
				int l1d_flush = cache_flush(l1d);
				int l2_flush = cache_flush(l2);
				cache_flush(l1i);
				stats.inst_cycles += l1d_flush + l2_flush;
				stats.flush_time += l1d_flush + l2_flush;
				stats.flushes++;
			}
			break;
		case 'W':
			cost = dispatch_write(l1d, address, bytesize);
			stats.write_cycles += cost;
			stats.writes++;
			break;
		case 'R':
			stats.read_cycles += dispatch_read(l1d , address, bytesize);
			stats.reads++;
			break;
		}
	}
}

void output_stats(struct stat_struct *result, struct mem_config *mem, struct cache *l1_i, struct cache *l1_d, struct cache *l2)
{
	printf("------------------------------------------------------------\n");
	printf("		<trace>.<config>		Simulation Results          \n");
	printf("------------------------------------------------------------\n");
	printf("\n");
	printf("Memory System:\n");
	printf("    Dcache Size = %d : ways = %d : block size = %d\n", l1_d->cache_size * l1_d->block_size, l1_d->assoc,l1_d->block_size);
	printf("    Icache Size = %d : ways = %d : block size = %d\n", l1_i->cache_size * l1_i->block_size, l1_i->assoc,l1_i->block_size);
	printf("    L2-cache Size = %d : ways = %d : block size = %d\n", l2->cache_size * l2->block_size, l2->assoc,l2->block_size);
	printf("    Memory ready time = %d : chunksize = %d : chunktime = %d\n", mem->mem_ready, mem->mem_chunksize, mem->mem_chunktime);
	printf("\n");

	printf("Execute Time = %ld; Total References = %ld\n", result->read_cycles + result->write_cycles + result->inst_cycles, result->reads + result->writes + result->insts);
	printf("Flush Time = %ld\n", result->flush_time);
	printf("Inst refs = %ld; Data refs = %ld\n",result->insts,result->reads + result->writes);
	printf("\n");

	printf("Number of references types: [Percentage]\n");
	printf("    Reads  =     %ld    [%.1f%%]\n", result->reads ,100*((double)result->reads)/(result->reads+result->writes+result->insts));
	printf("    Writes =     %ld    [%.1f%%]\n", result->writes,100*((double)result->writes)/(result->reads+result->writes+result->insts));
	printf("    Inst.  =     %ld    [%.1f%%]\n", result->insts,100*((double)result->insts)/(result->reads+result->writes+result->insts));
	printf("    Total  =     %ld\n",result->reads+result->writes+result->insts);
	printf("\n");

	unsigned long int extime = result->read_cycles + result->write_cycles + result->inst_cycles;
	printf("Total cycles for activities: [Percentage]\n");
	printf("    Reads  =     %ld    [%.1f%%]\n",result->read_cycles,100*((double)(result->read_cycles))/extime);
	printf("    Writes =     %ld    [%.1f%%]\n",result->write_cycles,100*((double)(result->write_cycles))/extime);
	printf("    Inst.  =     %ld    [%.1f%%]\n",result->inst_cycles,100*((double)(result->inst_cycles))/extime);
	printf("    Total  =     %ld\n",extime);
	printf("\n");

	printf("Average cycles for activiites: [Percentage]\n");
	printf("  Read =  %.1f; Write =  %.1f; Inst. =  %.1f\n",((double)result->read_cycles)/(result->reads),((double)result->write_cycles)/(result->writes),((double)extime)/(result->insts));
	printf("Ideal: Exec. Time = %ld; CPI =  %.1f\n",result->reads + result->writes + 2*result->insts,((double)result->reads+result->writes+2*result->insts)/(result->insts));
	unsigned long misaligned_exec = result->insts + l1_i->cache_stats.requests + l1_d->cache_stats.requests;
	printf("Ideal mis-aligned: Exec. Time = %ld; CPI = %.1f\n", misaligned_exec, ((double)misaligned_exec / result->insts));
	printf("\n");

	printf("Memory Level: L1i\n");
	printf("    Hit Count = %ld  Miss Count = %ld\n",l1_i->cache_stats.hits, l1_i->cache_stats.requests - l1_i->cache_stats.hits);
	printf("    Total Requests = %ld\n",l1_i->cache_stats.requests);
	printf("    Hit Rate = %.1f%%   Miss Rate = %.1f%%\n",100*((float)(l1_i->cache_stats.hits)/(l1_i->cache_stats.requests)),100*((float)(l1_i->cache_stats.requests - l1_i->cache_stats.hits)/(l1_i->cache_stats.requests)));
	printf("    Kickouts = %ld; Dirty Kickouts = %ld; Transfers = %ld\n",l1_i->cache_stats.kickouts,l1_i->cache_stats.dirty_kickouts,l1_i->cache_stats.flush_kickouts + l1_i->cache_stats.requests - l1_i->cache_stats.hits);
	printf("    Flush Kickouts  =  %ld\n", l1_i->cache_stats.flush_kickouts);
	printf("\n");

	printf("Memory Level: L1d\n");
	printf("    Hit Count = %ld  Miss Count = %ld\n",l1_d->cache_stats.hits,l1_d->cache_stats.requests - l1_d->cache_stats.hits);
	printf("    Total Requests = %ld\n",l1_d->cache_stats.requests);
	printf("    Hit Rate = %.1f%%   Miss Rate = %.1f%%\n",100*((float)(l1_d->cache_stats.hits)/(l1_d->cache_stats.requests)),100*((float)(l1_d->cache_stats.requests - l1_d->cache_stats.hits)/(l1_d->cache_stats.requests)));
	printf("    Kickouts = %ld; Dirty Kickouts = %ld; Transfers = %ld\n",l1_d->cache_stats.kickouts,l1_d->cache_stats.dirty_kickouts,l1_d->cache_stats.flush_kickouts + l1_d->cache_stats.requests - l1_d->cache_stats.hits );
	printf("    Flush Kickouts  =  %ld\n", l1_d->cache_stats.flush_kickouts);
	printf("\n");

	printf("Memory Level: L2\n");
	printf("    Hit Count = %ld  Miss Count = %ld\n",l2->cache_stats.hits,l2->cache_stats.requests - l2->cache_stats.hits);
	printf("    Total Requests = %ld\n",l2->cache_stats.requests);
	printf("    Hit Rate = %.1f%%   Miss Rate = %.1f%%\n",100*((float)(l2->cache_stats.hits)/(l2->cache_stats.requests)),100*((float)(l2->cache_stats.requests - l2->cache_stats.hits)/(l2->cache_stats.requests)));
	printf("    Kickouts = %ld; Dirty Kickouts = %ld; Transfers = %ld\n",l2->cache_stats.kickouts,l2->cache_stats.dirty_kickouts,l2->cache_stats.flush_kickouts + l2->cache_stats.requests - l2->cache_stats.hits);
	printf("    Flush Kickouts  =  %ld\n", l2->cache_stats.flush_kickouts);
	printf("\n");

	int l1_i_cost = ((l1_i->cache_size * l1_i->block_size * l1_i->assoc) / 4096) * (100 + 100 * log_2(l1_i->assoc));
	int l1_d_cost = ((l1_d->cache_size * l1_d->block_size * l1_d->assoc) / 4096) * (100 + 100 * log_2(l1_d->assoc));
	int l2_cost = (50 * (l2->cache_size * l2->block_size * l2->assoc)) / 65536 + 50 * log_2(l2->assoc);
	int memcost = log_2(mem->mem_ready / 30) * 200 + log_2(mem->mem_chunktime / 15) * 100 + 75;
	printf("L1 cache cost (Icache $%d) + (Dcache $%d) = $%d\n", l1_i_cost, l1_d_cost, l1_i_cost+l1_d_cost);
	printf("L2 cache cost = $%d;   Memory Cost = $%d\n", l2_cost, memcost);
	printf("Flushes = %d : Invalidates = %d\n", result->flushes, result->flushes);
	printf("\n");
}

int main(int argc, char **argv)
{
	int transfer_time = 5;
	int bus_width = 16;

	/* Load configuration from argv[1] if it exists */
	if (argc > 1) {
		load_config(argv[1], &l1_i, &l1_d, &l2, &mem, &transfer_time, &bus_width);
	}

	l1_i.transfer_time = transfer_time;
	l1_i.bus_width = bus_width;
	l1_d.transfer_time = transfer_time;
	l1_d.bus_width = bus_width;

	init_cache(&l1_i);
	init_cache(&l1_d);
	init_cache(&l2);

	l1_i.backend = &l2;
	l1_d.backend = &l2;
	l2.backend = NULL;

	process_trace(&l1_i, &l1_d, &l2, &mem);

	output_stats(&stats, &mem, &l1_i, &l1_d, &l2);

	return 0;
}
