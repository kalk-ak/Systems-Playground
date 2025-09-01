#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Cache.h"

// Error codes for different types of invalid input
#define INVALID_COMMAND_LINE 1
#define INVALID_SET_NUM 2
#define INVALID_BLOCK_NUM 3
#define INVALID_BLOCK_SIZE 4
#define INVALID_MISS_TYPE 5
#define INVALID_HIT_TYPE 6
#define INVALID_EVICTION 7
#define INVALID_OPERATOR 8
#define INVALID_ADDRESS 9

// Helper function to convert a string to lowercase
std::string to_lower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Helper function to check if a number is a power of 2
bool is_power_of_2(uint32_t n) { return (n & (n - 1)) == 0; }

int main(int argc, char** argv) {
    // Expect exactly 7 arguments: program name + 6 user inputs
    if (argc != 7) {
        std::cerr << "Command Line Argument Format: " << argv[0]
                  << " <num_sets> <num_blocks> <block_size> "
                  << "<miss_type> <hit_type>  <eviction>" << std::endl;
        return INVALID_COMMAND_LINE;
    }

    int set_num, block_num, block_size;
    std::string miss_type, hit_type, eviction;

    try {
        // Parse integer arguments
        set_num = std::stoi(argv[1]);
        block_num = std::stoi(argv[2]);
        block_size = std::stoi(argv[3]);

        // Normalize string arguments to lowercase
        miss_type = to_lower(argv[4]);
        hit_type = to_lower(argv[5]);
        eviction = to_lower(argv[6]);
    } catch (const std::exception& e) {
        std::cerr
            << "Error: First three arguments must be integers (powers of 2)."
            << std::endl;
        return INVALID_COMMAND_LINE;
    }

    // Validate that integers are positive
    if (set_num <= 0 || block_num <= 0 || block_size <= 0) {
        std::cerr << "Error: All numeric arguments must be positive integers."
                  << std::endl;
        return INVALID_COMMAND_LINE;
    }

    // Convert to unsigned for power-of-2 check
    uint32_t u_set_num = static_cast<uint32_t>(set_num);
    uint32_t u_block_num = static_cast<uint32_t>(block_num);
    uint32_t u_block_size = static_cast<uint32_t>(block_size);

    // Check if number of sets is a power of 2
    if (!is_power_of_2(u_set_num)) {
        std::cerr << "Error: Number of sets (" << set_num
                  << ") must be a power of 2." << std::endl;
        return INVALID_SET_NUM;
    }

    // Check if block size is at least 4 and a power of 2
    if (block_size < 4 || !is_power_of_2(u_block_size)) {
        std::cerr << "Error: Block size (" << block_size
                  << ") must be at least 4 and a power of 2." << std::endl;
        return INVALID_BLOCK_SIZE;
    }

    // Validate hit type
    if (hit_type != "write-through" && hit_type != "write-back") {
        std::cerr << "Error: Hit type must be 'write-through' or 'write-back'."
                  << std::endl;
        return INVALID_HIT_TYPE;
    }

    // Validate miss type
    if (miss_type != "write-allocate" && miss_type != "no-write-allocate") {
        std::cerr << "Error: Miss type must be 'write-allocate' or "
                     "'no-write-allocate'."
                  << std::endl;
        return INVALID_MISS_TYPE;
    }

    if (hit_type == "write-back" && miss_type == "no-write-allocate") {
        std::cerr
            << "Error: Can't have write-back and no-write-allocate together."
            << std::endl;
        return INVALID_MISS_TYPE;
    }

    // Validate eviction policy
    if (eviction != "lru" && eviction != "fifo") {
        std::cerr << "Error: Eviction policy must be 'lru' or 'fifo'."
                  << std::endl;
        return INVALID_EVICTION;
    }

    // Translate string options to boolean flags
    bool miss_write_type = (miss_type == "write-allocate") ? true : false;
    bool hit_write_type = (hit_type == "write-back") ? true : false;
    bool eviction_type = (eviction == "lru") ? true : false;

    // Initialize the cache simulation
    Cache simulation = Cache(u_set_num, u_block_num, u_block_size,
                             miss_write_type, hit_write_type, eviction_type);

    // Begin simulation
    std::cout << "Running the simulation." << std::endl;

    std::string line;  // Input line buffer

    uint32_t run = 0;
    // Read lines from standard input
    while (std::getline(std::cin, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);

        std::string op,
            hex_address;  // Operation type and memory address
        int value;

        if (!(iss >> op >> hex_address >> value)) {
            std::cerr << "Invalid input format in line. Simulation run " << run
                      << std::endl;
            return INVALID_COMMAND_LINE;
        }

        op = to_lower(op);  // Normalize operator string

        if (op.size() != 1 && op != "l" && op != "s") {
            std::cerr << "operator must be of length 1. Either l for load or s "
                         "for store. Simulation run "
                      << run << std::endl;
            return INVALID_OPERATOR;
        }

        // Convert address from hex string to unsigned int
        uint32_t address;
        try {
            address = std::stoul(hex_address, nullptr, 16);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid address: address must be a 32-bit hex "
                         "representation. "
                      << "Simulation run " << run << std::endl;
            return INVALID_ADDRESS;
        }

        // Perform the cache operation
        if (op == "l") {
            simulation.load(address);
        } else {
            simulation.store(address);
        }
        ++run;
    }

    // Output simulation summary
    std::cout << "Total loads: " << simulation.get_loads() << "\n"
              << "Total stores: " << simulation.get_stores() << "\n"
              << "Load hits: " << simulation.get_load_hits() << "\n"
              << "Load misses: " << simulation.get_load_misses() << "\n"
              << "Store hits: " << simulation.get_store_hits() << "\n"
              << "Store misses: " << simulation.get_store_misses() << "\n"
              << "Total cycles: " << simulation.get_cycles() << std::endl;

    return EXIT_SUCCESS;
}
