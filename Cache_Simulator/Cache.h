#ifndef CACHE_H
#define CACHE_H

#include <sys/types.h>

#include <cstdint>
#include <map>
#include <vector>

/**
 * Class representing a configurable cache memory simulation.
 */
class Cache {
public:
    /**
     * Constructor to initialize the cache with specified parameters.
     *
     * @param sets Number of sets in the cache.
     * @param blocks Number of blocks (slots) per set.
     * @param bytes Number of bytes per block.
     * @param miss_write_type True for Write-Allocate, False for
     * No-Write-Allocate.
     * @param hit_write_type True for Write-Back, False for Write-Through.
     * @param eviction_type True for LRU, False for FIFO.
     */
    Cache(uint32_t sets, uint32_t blocks, uint32_t bytes, bool miss_write_type,
          bool hit_write_type, bool eviction_type);

    // destructor
    ~Cache();

    /**
     * Loads an address from the cache or memory. Handles eviction and loading.
     *
     * @param address Address to be loaded.
     * @return True if the load resulted in a cache hit, otherwise false.
     */
    bool load(uint32_t address);

    /**
     * Stores to an address in the cache or memory. Handles eviction and
     * updating.
     *
     * @param address Address to be stored.
     * @return True if the store resulted in a cache hit, otherwise false.
     */
    bool store(uint32_t address);

    // ---------------------- (Getters for private variables)
    // ------------------------------

    /**
     * @return  Total cache loads
     */
    uint32_t get_loads() { return total_loads; }

    /**
     * @return Total cache store
     */
    uint32_t get_stores() { return total_stores; }

    /**
     * @retrun Total cache load hits
     */
    uint32_t get_load_hits() { return load_hits; }

    /**
     * @return Total cache load misses
     */
    uint32_t get_load_misses() { return load_misses; }

    /**
     * @return Total cache store hits
     */
    uint32_t get_store_hits() { return store_hits; }

    /**
     * @return Total cache store misses
     */
    uint32_t get_store_misses() { return store_misses; }

    /**
     * @return Total cycles in cache simulation
     */
    uint32_t get_cycles() { return total_cycles; }

private:
    // Simulated clock time, updated on each load/store operation
    int current_time = 0;

    /**
     * Structure representing a cache block (slot).
     */
    struct Slot {
        uint32_t tag;
        bool valid;
        bool dirty;
        uint32_t load_ts;    // Time when block was loaded (for FIFO)
        uint32_t access_ts;  // Last access time (for LRU)

        // slot constructor
        Slot(uint32_t tag, bool valid, bool dirty, uint32_t load_ts,
             uint32_t access_ts)
            : tag(tag),
              valid(valid),
              dirty(dirty),
              load_ts(load_ts),
              access_ts(access_ts) {}
    };

    /**
     * Structure representing a cache set, containing multiple slots.
     * Uses a map for the underlaying data structure becasue we could
     * efficiently look up a tag in the set
     */
    struct Set {
        std::map<uint32_t, Slot *> index;
    };

    // Cache data: collection of sets
    std::vector<Set> cache;

    // Evicts a slot from the cache and loads a new tag
    Slot *evict(Slot *s, uint32_t new_tag);

    //  Used for populating the cache until max size is reached in a set
    //  Attempts to add a slot to a set if capacity allows
    Slot *create_slot(uint32_t tag);

    // Extracts offset bits from an address
    uint32_t get_offset(uint32_t address);

    // Extracts index bits from an address
    uint32_t get_index(uint32_t address);

    // Extracts tag bits from an address
    uint32_t get_tag(uint32_t address);

    // Configuration parameters
    const uint32_t num_sets;          // Number of sets in the cache
    const uint32_t num_slots;         // Number of blocks per set
    const uint32_t num_bytes;         // Number of bytes per block
    const uint32_t writes_per_block;  // The number of times it takes to
    //    write
    // block to memory

    const bool
        miss_write_type;  // True: Write-Allocate, False: No-Write-Allocate
    const bool hit_write_type;  // True: Write-Back, False: Write-Through
    const bool eviction_type;   // True: LRU, False: FIFO

    // We get these using log2 of the Configuration parameters
    const uint32_t offset_size;  // Number of bits for offset
    const uint32_t index_size;   // Number of bits for index
    const uint32_t tag_size;     // Number of bits for tag

    // Cache statistics
    uint32_t total_loads = 0;
    uint32_t total_stores = 0;
    uint32_t load_hits = 0;
    uint32_t load_misses = 0;
    uint32_t store_hits = 0;
    uint32_t store_misses = 0;
    uint32_t total_cycles = 0;

    // Timing costs (in cycles)
    const uint32_t cache_cost = 1;     // Cache access cost
    const uint32_t memory_cost = 100;  // Memory access cost
};

#endif  // CACHE_H
