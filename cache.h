#ifndef CACHE_H
#define CACHE_H

#include <vector>

class Cache {
public:
private:
  int num_sets =
      -1; // stores the number of sets in this cache. Initialized to zero
  int num_block = -1; // stores the number of blocks in each set

  int num_bytes = -1; // stores the number of bytes in each block

  /**
   * Flag to know what kind of write type happens in a Cache miss
   * (true) for Write-Allocate
   * (false) for No-Write-Allocate
   */
  bool miss_write_type = false;

  /**
   * Flag to know what kind of write type happens in a Cache hit
   * (true) for Write-Back
   * (false) for Write-Through
   */
  bool hit_write_type = false;

  /**
   * Flag to know what kind of eviction happens for replacements
   * (true) for LRU
   * (false) for FIFo
   */
  bool eviction_type = true; // defaults to LRU

  // stores the total loads to this cache
  int total_loads = 0;

  // stores the total stores to this cache
  int total_stores = 0;

  // stores the load hits and misses for this cache
  int load_hits = 0;
  int load_misses = 0;

  // stores the store hits and misses for this cache
  int store_hits = 0;
  int store_misses = 0;

  const int cache_cost = 1;    // cost to store/read to cache is 1 cycle
  const int memory_cost = 100; // cost to store/load to memory is 100 cycles
};

#endif // !CACHE_H
#define CACHE_H
