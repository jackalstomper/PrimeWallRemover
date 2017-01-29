#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <string>
#include "utility.hpp"
#include "area_collision.hpp"

struct MreaResource
{
    uint32_t file_id;
    uint32_t offset;

    void read(std::fstream &file)
    {
        file.read(reinterpret_cast<char*>(&file_id), sizeof(uint32_t));
        file_id = Utility::byteSwap32(file_id);
        file.seekg(4, std::ios::cur);
        file.read(reinterpret_cast<char*>(&offset), sizeof(uint32_t));
        offset = Utility::byteSwap32(offset);
    }
};

void mapPak(const std::string &file_name)
{
    std::cout << "Opening " << file_name << '\n';
    std::fstream file(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!file)
        throw std::runtime_error("Couldn't open file " + file_name);

    uint32_t magic;
    file.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
    if (Utility::byteSwap32(magic) != 0x0030005)
        throw std::runtime_error("Invalid pak file encountered: " + file_name);

    // move to end of pak header
    file.seekg(0x8);

    // we're at the name resource table, get past it so we can get to the (unnamed?) resource table
    uint32_t resource_count;
    file.read(reinterpret_cast<char*>(&resource_count), sizeof(uint32_t));
    resource_count = Utility::byteSwap32(resource_count);

    // read through named resources
    uint32_t name_length;
    uint32_t i;
    for (i = 0; i < resource_count; ++i) {
        file.seekg(8, std::ios::cur);
        file.read(reinterpret_cast<char*>(&name_length), sizeof(uint32_t));
        file.seekg(Utility::byteSwap32(name_length), std::ios::cur);
    }

    // we're at the resource table
    char file_type[5];
    MreaResource resource;
    file_type[4] = '\0';
    file.read(reinterpret_cast<char*>(&resource_count), sizeof(uint32_t));
    resource_count = Utility::byteSwap32(resource_count);
    std::cout << "Found " << resource_count << " resources in pak\n";
    std::vector<MreaResource> resources;
    uint32_t compression_flag;

    for (i = 0; i < resource_count; ++i) {
        file.read(reinterpret_cast<char*>(&compression_flag), sizeof(uint32_t));
        compression_flag = Utility::byteSwap32(compression_flag);
        file.read(file_type, 4);
        if (std::strcmp(file_type, "MREA") == 0) {
            resource.read(file);
            resources.push_back(resource);
        } else {
            // skip ahead to next resource
            file.seekg(12, std::ios::cur);
        }
    }

    // go to each MREA resource found in the pak and modify its collision data
    for (auto i = resources.begin(); i != resources.end(); ++i) {
        std::cout << std::hex;
        std::cout << "Modifying " << i->file_id << ".MREA\n";
        std::cout << std::dec;
        file.seekg(i->offset);
        MREA::mapMrea(file);
    }
}

int main()
{
    std::string root;
    std::cout << "Enter Metroid Prime Root Directory: ";
    std::getline(std::cin, root);

    const char path_sep =
        #ifdef _WIN32
            '\\';
        #else
            '/';
        #endif

    std::string path;
    for (int i = 1; i <= 8; ++i) {
        path = root + path_sep + std::string("Metroid") + std::to_string(i) + ".pak";
        mapPak(path);
    }

    std::cout << "Done!\n";
    return 0;
}
