I started by laying out an object-oriented approach that follows principles such as abstraction and encapsulation. To store the cache blocks, I used structs to represent metadata (since actual data is not necessary for this simulation). These structs are stored within each set, and each set uses a hash map  to manage its blocks.

The key in the map is the tag, and the value is a pointer to a slot stored on the heap. This structure provides fast lookups, which becomes especially beneficial when dealing with a large number of sets, such as in a fully associative cache. It is also memory-efficient, as slots are only allocated when needed. Slots are not deleted and reallocated during eviction; instead, their metadata is only updated.

I chose to use heap allocation for the slots to avoid stack overflows in scenarios with a large number of blocks. The heap allows for more scalable memory usage in such cases.

Time in the simulation is managed using an unsigned integer that increments with each load and store operation. This timestamp supports both the FIFO and LRU replacement policies, both of which are implemented in the simulator.

The load and store methods require only the address as input. They internally compute the tag, offset, and index using helper functions. The index identifies the set in the main cache array, the tag is used to search for the block within that set, and the offset—although not critical for this simulation—can be useful to determine word alignment or sub-block access.




Best Cache Report:

I let the simulator run multiple times with different cache systems. I tested fully associative caches with a set of 1 and the set having 206 number of blocks, as well as set-associative caches with 10 and 100 sets. I also tested direct maped cache configurations that had as many as (1024 / 4) blocks with a block size of 4 bytes. After comparing hit rates and cycle counts across these different setups, I found the following trends. The storeage size that i used is 1kb (1024 bytes) of storeage for each simulation runs.


--- LRU vs FIFO

The experiments consistently showed that LRU outperforms FIFO across most trace files. With LRU, frequently accessed blocks are retained longer, leading to fewer cache misses. This advantage is especially clear as the number of operations increases in the simulator. The LRU and FIFO are only effective if the cache is being utilized, meaning the simulator isn't just updating the memory. A reason why the FIFO might perform worse than the LRU is becaue it is just evicting blocks by their order they were added and this doesn't necessarily tell how much frequently that block of data is being accessed. 

---Write Back vs Write Through

The cycle counts reported in the simulator reveal that write-back caching is superior to write-through, especially for write-intensive tasks. Write-back defers costly memory writes until eviction (costing only 1 memory cost ) by keeping updates in the cache. This results in a significant reduction in total cycles on the write.trace compared to the immediate memory writes required by write-through.


--- Write-Allocate vs No Write Allocate

My tests also showed that write-allocate performs better than no write allocate. With write-allocate, store misses result in blocks being loaded into the cache, which benefits both future reads and writes. This strategy is particularly effective in mixed workloads like swim.trace and the gcc.trace, where caching store misses helps balance both read and write operations.


--- Combining Write-Allocate with Write-Back

The combination of write-allocate and write-back minimizes expensive memory writes while maintaining high cache hit rates. This configuration significantly delivered lower clock cycle counts across all traces. Direct memory writes, which cost 100 cycles per operation, were significantly reduced, making this combo the best overall in scenarios with heavy writes.


--- Write-Through and No Write Allocate vs. Other Combinations

On the other hand, setups using write-through with no write allocate showed the worst performance, with the highest cycle counts. Frequent direct writes to memory severely impact overall performance, especially in the write.trace tests, where write latency is critical.


NOTE: 
These observations become more evident as the operations in the cache increases like in gcc.trace



--- Read vs. Write Performance

    Read Performance: In read-intensive scenarios, like those in read.trace, configurations that maximize hit rates (using LRU eviction and write-allocate) perform best. The lower cycle counts reflect minimal delays since data is already cached.

    Write Performance: For write-heavy traces such as write.trace, reducing the number of direct memory writes is key. The write-back strategy performs better here, as it delays writes until absolutely necessary, dramatically lowering cycle counts compared to write-through methods.
