#include "Cache.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
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
    // Initialize each set with an empty map of tag-to-slot pointers
    cache = std::vector<Set>(num_sets, Set{std::map<uint32_t, Slot*>()});
}

// destructor
Cache::~Cache() {
    // Free all dynamically allocated slots in each set
    for (std::vector<Set>::iterator it = cache.begin(); it != cache.end();
         ++it) {
        Set& current_set = *it;

        // Deallocate each slot in the set
        for (auto& pair : current_set.index) {
            delete pair.second;  // Free memory for each slot
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
    ++total_loads;   // Count the load operation
    ++current_time;  // Advance time for LRU/FIFO tracking

    // Extract offset, index, and tag from the address
    uint32_t index = get_index(address);
    uint32_t tag = get_tag(address);

    // Get the set corresponding to the index
    Set& current = cache[index];

    // Check for a hit
    if (current.index.find(tag) != current.index.end()) {
        ++load_hits;  // Cache hit

        // Update access time for LRU
        current.index[tag]->access_ts = current_time;

        total_cycles += cache_cost;  // Cost of accessing the cache
        return true;
    } else {
        ++load_misses;  // Cache miss

        // Add memory access and cache access costs
        total_cycles += cache_cost + (memory_cost * writes_per_block);

        if (current.index.size() < num_slots) {
            // Space available — insert new slot
            current.index[tag] = create_slot(tag);
        } else {
            // Set is full — need to evict
            uint32_t evict_tag;

            if (eviction_type) {
                // Use LRU policy
                uint32_t least_recent = UINT32_MAX;

                // find the least_recent set
                for (const auto& pair : current.index) {
                    Slot* current_slot = pair.second;

                    if (current_slot->access_ts < least_recent) {
                        evict_tag = pair.first;
                        least_recent = current_slot->access_ts;
                    }
                }

                assert(least_recent != UINT32_MAX);

            } else {
                // Use FIFO policy
                uint32_t oldest = UINT32_MAX;

                // find the onldest set
                for (const auto& pair : current.index) {
                    Slot* current_slot = pair.second;

                    if (current_slot->load_ts < oldest) {
                        evict_tag = pair.first;
                        oldest = current_slot->load_ts;
                    }
                }

                assert(oldest != UINT32_MAX);
            }

            // Replace evicted slot
            current.index[tag] = evict(current.index[evict_tag], tag);
            current.index.erase(evict_tag);
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
    ++total_stores;  // Count the store operation
    ++current_time;  // Advance time for LRU/FIFO tracking

    // Extract index and tag
    uint32_t index = get_index(address);
    uint32_t tag = get_tag(address);

    Set& current = cache[index];

    // Check for cache hit
    if (current.index.find(tag) != current.index.end()) {
        ++store_hits;  // Cache hit

        current.index[tag]->access_ts = current_time;

        if (hit_write_type) {
            // Write-back: write to cache, mark dirty
            total_cycles += cache_cost;
            current.index[tag]->dirty = true;
        } else {
            // Write-through: write to cache and memory
            total_cycles += (cache_cost + memory_cost);
        }

        return true;
    } else {
        ++store_misses;  // Cache miss

        if (!miss_write_type) {
            // No-Write-Allocate: only write to memory
            total_cycles += memory_cost;

        } else {
            if (current.index.size() < num_slots) {
                // Space available — create slot
                current.index[tag] = create_slot(tag);
            } else {
                // Set full — evict based on policy
                uint32_t evict_tag;

                if (eviction_type) {
                    // LRU policy
                    uint32_t least_recent = UINT32_MAX;

                    for (const auto& pair : current.index) {
                        Slot* current_slot = pair.second;
                        if (current_slot->access_ts < least_recent) {
                            evict_tag = pair.first;
                            least_recent = current_slot->access_ts;
                        }
                    }

                    assert(least_recent != UINT32_MAX);
                } else {
                    // FIFO policy
                    uint32_t oldest = UINT32_MAX;

                    for (const auto& pair : current.index) {
                        Slot* current_slot = pair.second;
                        if (current_slot->load_ts < oldest) {
                            evict_tag = pair.first;
                            oldest = current_slot->load_ts;
                        }
                    }

                    assert(oldest != UINT32_MAX);
                }

                current.index[tag] = evict(current.index[evict_tag], tag);
                current.index.erase(evict_tag);
            }

            // Apply write-back or write-through after allocation
            if (hit_write_type) {
                // Write-back + Write-Allocate
                total_cycles += (writes_per_block * memory_cost) + cache_cost;
                current.index[tag]->dirty = true;
            } else {
                // Write-through + Write-Allocate
                total_cycles +=
                    (writes_per_block * memory_cost) + cache_cost + memory_cost;

                current.index[tag]->dirty = false;
            }
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
        // If write-back and dirty, write back to memory on eviction
        total_cycles +=
            (memory_cost * writes_per_block);  // Cost to write back to memory
    }
    s->tag = new_tag;
    s->valid = false;
    s->dirty = false;
    s->load_ts = current_time;
    s->access_ts = current_time;

    return s;
}

/**
 * Creates a new slot for the given tag.
 * Only called when a set has available space.
 *
 * @param tag The tag associated with the new slot.
 * @return A pointer to the newly created slot.
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
