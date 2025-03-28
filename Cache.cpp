#include "Cache.h"

#include <sys/types.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <map>

/**
 * Constructor to initialize the cache with specified parameters.
 *
 * @param sets Number of sets in the cache.
 * @param blocks Number of blocks (slots) per set.
 * @param bytes Number of bytes per block.
 * @param miss_write_type True for Write-Allocate, False for No-Write-Allocate.
 * @param hit_write_type True for Write-Back, False for Write-Through.
 * @param eviction_type True for LRU, False for FIFO.
 */
Cache::Cache(uint32_t sets, uint32_t blocks, uint32_t bytes,
             bool miss_write_type, bool hit_write_type, bool eviction_type)
    : num_sets(sets),
      num_slots(blocks),
      num_bytes(bytes),
      writes_per_block(num_bytes / 4),
      miss_write_type(miss_write_type),
      hit_write_type(hit_write_type),
      eviction_type(eviction_type),
      offset_size(log2(bytes)),
      index_size(log2(num_sets)),
      tag_size(32 - (offset_size + index_size)) {
    // Initialize cache with the specified number of sets and slots per set
    cache = std::vector<Set>(num_sets, Set{std::map<uint32_t, Slot*>()});
}

// destructor
Cache::~Cache() {
    // free all slots in each set
    for (std::vector<Set>::iterator it = cache.begin(); it != cache.end();
         ++it) {
        Set& current_set = *it;

        // Deallocate each value:
        for (auto& pair : current_set.index) {
            delete pair.second;  // Free the memory allocated for the object.
        }
    }
}

/**
 * Loads an address from the cache or memory.
 * Handles tag extraction, set lookup, hit/miss detection,
 * LRU/FIFO eviction, and updating cache state.
 *
 * @param address The memory address to load.
 * @return True if the load resulted in a cache hit, otherwise false.
 */
bool Cache::load(uint32_t address) {
    ++total_loads;   // increment the total number of load operations
    ++current_time;  // increment the current cache time for tracking recency

    // get the offset, index, and tag from the address
    uint32_t index = get_index(address);
    uint32_t tag = get_tag(address);

    // get a reference to the set that this index maps to
    Set& current = cache[index];

    // check if the tag already exists in the set (i.e., a cache hit)
    if (current.index.find(tag) != current.index.end()) {
        ++load_hits;  // tag is already in cache, so we register a hit

        // update the access time of the slot for LRU tracking
        current.index[tag]->access_ts = current_time;

        total_cycles += cache_cost;  // only cost is reading from cache
        return true;
    } else {
        ++load_misses;  // tag not in cache, so this is a miss

        // update total cycles: cost of reading from memory + reading from cache
        total_cycles += cache_cost + (memory_cost * writes_per_block);

        if (current.index.size() < num_slots) {
            // the set still has space, so insert a new slot for this tag
            current.index[tag] = create_slot(tag);
        } else {
            // the set is full — need to evict a slot based on replacement
            // policy
            uint32_t evict_tag;

            if (eviction_type) {
                // if eviction_type is true, use LRU replacement policy
                uint32_t least_recent = UINT32_MAX;

                // loop through all slots in the current set to find the least
                // recently used
                for (const auto& pair : current.index) {
                    Slot* current_slot = pair.second;

                    // if this slot has been accessed earlier than the current
                    // minimum, update
                    if (current_slot->access_ts < least_recent) {
                        evict_tag = pair.first;
                        least_recent = current_slot->access_ts;
                    }
                }

                // ensure we found a slot to evict
                assert(least_recent != UINT32_MAX);

            } else {
                // if eviction_type is false, use FIFO replacement policy
                uint32_t oldest = UINT32_MAX;

                // loop through all slots in the current set to find the oldest
                // loaded one
                for (const auto& pair : current.index) {
                    Slot* current_slot = pair.second;

                    // if this slot was loaded earlier than the current oldest,
                    // update
                    if (current_slot->load_ts < oldest) {
                        evict_tag = pair.first;
                        oldest = current_slot->load_ts;
                    }
                }

                // ensure we found a slot to evict
                assert(oldest != UINT32_MAX);
            }

            // evict the old slot and insert a new one with the current tag
            current.index[tag] =
                evict(current.index[evict_tag], tag);  // reuse or create slot
            current.index.erase(
                evict_tag);  // remove the evicted slot's key from the set
        }
        return false;
    }
}

/**
 * Stores to an address in the cache or memory.
 * Handles lookup, write policies, and updates.
 *
 * @param address The memory address to store.
 * @return True if store is a cache hit, otherwise false.
 */
bool Cache::store(uint32_t address) {
    ++total_stores;  // increment total number of store operations
    ++current_time;  // update current cache time

    // extract  index, and tag from the address
    uint32_t index = get_index(address);
    uint32_t tag = get_tag(address);

    // get a reference to the set that this index maps to
    Set& current = cache[index];

    // check if the tag already exists in the set (cache hit)
    if (current.index.find(tag) != current.index.end()) {
        ++store_hits;  // cache hit

        // update the access time for LRU policy
        current.index[tag]->access_ts = current_time;

        // handle write-hit behavior depending on policy
        if (hit_write_type) {
            // write-back: update cache only and mark as dirty
            total_cycles += cache_cost;
            current.index[tag]->dirty = true;
        } else {
            // write-through: update cache and memory immediately (no dirty bit)
            total_cycles += (cache_cost + memory_cost);
        }

        return true;
    } else {
        ++store_misses;  // cache miss

        // insert a new slot if space is available in the set
        if (current.index.size() < num_slots) {
            current.index[tag] = create_slot(tag);
        } else {
            // set is full — need to evict a slot based on replacement policy
            uint32_t evict_tag;

            if (eviction_type) {
                // LRU policy: evict the least recently accessed slot
                uint32_t least_recent = UINT32_MAX;

                // loop through all slots to find the LRU one
                for (const auto& pair : current.index) {
                    Slot* current_slot = pair.second;
                    if (current_slot->access_ts < least_recent) {
                        evict_tag = pair.first;
                        least_recent = current_slot->access_ts;
                    }
                }

                assert(least_recent != UINT32_MAX);
            } else {
                // FIFO policy: evict the oldest loaded slot
                uint32_t oldest = UINT32_MAX;

                // loop through all slots to find the oldest one
                for (const auto& pair : current.index) {
                    Slot* current_slot = pair.second;
                    if (current_slot->load_ts < oldest) {
                        evict_tag = pair.first;
                        oldest = current_slot->load_ts;
                    }
                }

                assert(oldest != UINT32_MAX);
            }

            // evict the selected slot and replace it with a new one
            current.index[tag] = evict(current.index[evict_tag], tag);
            current.index.erase(evict_tag);
        }

        // handle write-miss based on write-allocate and write back policy
        if (miss_write_type && hit_write_type) {
            // write-back + write-allocate:
            // load block into cache, write to cache, delay memory update
            total_cycles += (writes_per_block * memory_cost) + cache_cost;
            current.index[tag]->dirty = true;
        } else if (miss_write_type) {
            // write-through + write-allocate:
            // load block into cache, write to cache, write to memory
            total_cycles +=
                (writes_per_block * memory_cost) + cache_cost + memory_cost;
        } else {
            // no-write-allocate:
            // write directly to memory, do not cache
            total_cycles += memory_cost;
        }

        return false;
    }
}

/**
 * Evicts the given slot and replaces its metadata with a new tag.
 *
 * @param s The cache slot to evict.
 * @param new_tag The tag to assign to the slot after eviction.
 */
Cache::Slot* Cache::evict(Slot* s, uint32_t new_tag) {
    if (s->dirty and hit_write_type) {
        // if the cache is dirty and write back make sure to write back to
        // memory when evicting
        total_cycles += (memory_cost *
                         writes_per_block);  // cost for writing back to memory
    }
    s->tag = new_tag;
    s->valid = false;
    s->dirty = false;
    s->load_ts = current_time;
    s->access_ts = current_time;

    return s;
}

/**
 * Attempts to create a slot in the set corresponding to the address.
 * Only called until set is full. Then it is only evicted.
 * @param address The memory address that triggered the creation.
 * @return True if a new slot was successfully created, otherwise false.
 */
Cache::Slot* Cache::create_slot(uint32_t tag) {
    Cache::Slot* new_slot =
        new Cache::Slot(tag, false, false, current_time, current_time);
    return new_slot;
}

/**
 * Extracts the offset part of the address.
 *
 * @param address The full 32-bit memory address.
 * @return The offset portion.
 */
uint32_t Cache::get_offset(uint32_t address) {
    uint32_t offset = address & ((1 << offset_size) - 1);
    return offset;
}

/**
 * Extracts the index part of the address.
 *
 * @param address The full 32-bit memory address.
 * @return The index portion.
 */
uint32_t Cache::get_index(uint32_t address) {
    uint32_t index = (address >> offset_size) & ((1 << index_size) - 1);
    return index;
}

/**
 * Extracts the tag part of the address.
 *
 * @param address The full 32-bit memory address.
 * @return The tag portion.
 */
uint32_t Cache::get_tag(uint32_t address) {
    uint32_t tag = (address >> (offset_size + index_size));
    return tag;
}
