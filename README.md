# cache-sim

**The code in this repository is provided as-is, with no warranty, guarantees, liability on the author's part, or likely future updates.**

**Plezkun changes this project every semester, and he will fail you if you copy others' work, in accordance with the honor code.** 


ECEN 4593, Spring 2015, University of Colorado at Boulder 
 Cache Similation Final Project

Authors: Edward Zhu, Maxwell Russek 



### How to Run the Simulation

  make
  
  zcat < /path/to/trace | ./cache-sim <CONFIG>

### Folder layout

* main.c - Contains main procedures and printing output results
* cache.c - Contains main cache simulation code 
* config.c - config file parser
* Final_Report.pdf - Final Written Report
* Codebase_Results.pdf - All code and simulations results
* configs/ - Directory of cache configuration files
* output/ - Directory of all simulation outputs
* cache-data.csv - CSV formatted simulation results
* run-config.sh - Bash script to run a configuration on all traces.
* run-one-config.sh - Bash script to run a configuration on one trace.