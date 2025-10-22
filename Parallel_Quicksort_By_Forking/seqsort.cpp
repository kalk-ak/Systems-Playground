#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

void execute(int argc, char **argv)
{
    std::filesystem::path p(argv[1]);

    // Open the file for input
    std::ifstream in(p, std::ios_base::binary);
    if (!in.is_open())
    {
        std::stringstream ss;
        ss << "Could not open '" << p << "'";
        throw std::runtime_error(ss.str());
    }

    // Determine file size and number of elements
    auto file_size = std::filesystem::file_size(p);
    size_t num_elements = file_size / sizeof(int64_t);

    // Allocate memory
    std::unique_ptr<int64_t[]> buf(new int64_t[num_elements]);

    if (!in.read(reinterpret_cast<char *>(buf.get()), num_elements * sizeof(int64_t)))
        throw std::runtime_error("Could not read data from file");

    in.close();

    std::sort(buf.get(), buf.get() + num_elements);

    std::ofstream out(p, std::ios_base::binary);
    if (!out.write(reinterpret_cast<char *>(buf.get()), num_elements * sizeof(int64_t)))
        throw std::runtime_error("Could not write sorted data");

    // Success! the std::unique_ptr will delete the array
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./seqsort <filename>\n";
        exit(1);
    }

    try
    {
        execute(argc, argv);
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        exit(1);
    }

    return 0;
}
