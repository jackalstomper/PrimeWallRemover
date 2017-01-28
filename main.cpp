#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <array>

#ifdef _MSC_VER
#   include <intrin.h>
#endif

namespace {

uint32_t byteSwap32(uint32_t value)
{
#ifdef _MSC_VER
    return _byteswap_ulong(value);
#else
    return __builtin_bswap32(value);
#endif
}

}

int main() {
    std::string file_name("test.MREA");
    std::ifstream file(file_name, std::ios::binary);
    if (!file) {
        std::cout << "failed\n";
        return 0;
    }

    uint32_t magic;
    file.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
    if (byteSwap32(magic) != 0xDEADBEEF) {
        std::cout << "Invalid MREA file encountered: " << file_name << '\n';
        return 1;
    }

    // navigate to collision section of MREA
    // see http://www.metroid2002.com/retromodding/wiki/MREA_%28Metroid_Prime%29
    uint32_t data_section_count;
    uint32_t collision_index;
    uint32_t collision_section_size;
    std::vector<uint32_t> data_section_sizes;

    file.seekg(0x3C);
    file.read(reinterpret_cast<char*>(&data_section_count), sizeof(uint32_t));
    data_section_count = byteSwap32(data_section_count);

    file.seekg(0x48);
    file.read(reinterpret_cast<char*>(&collision_index), sizeof(uint32_t));
    collision_index = byteSwap32(collision_index);

    file.seekg(0x60);
    uint32_t size;
    data_section_sizes.reserve(data_section_count);
    for (uint32_t i = 0; i < data_section_count; ++i) {
        file.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
        data_section_sizes.push_back(byteSwap32(size));
    }

    // at the end of MREA header
    // navigate to next 32 byte boundry
    uint32_t pos = file.tellg();
    file.seekg(((pos + 32u - 1u) & ~(32u - 1u)));

    // navigate through data sections until we get to the collision section
    uint32_t offset = 0;
    for (uint32_t i = 0; i < collision_index; ++i)
        offset += data_section_sizes[i];
    
    file.seekg(offset, std::ios::cur);

    // should be at collision area of file
    file.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
    if (byteSwap32(magic) != 0x01000000) {
        // we got lost somehow and we no longer know where we are - abort!
        std::cout << "We got lost while navigating to collision section of MREA file " << file_name << "\nAborting.\n";
        return 1;
    }

    return 0;
}