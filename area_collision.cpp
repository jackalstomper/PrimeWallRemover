#include "area_collision.hpp"
#include "utility.hpp"
#include <vector>
#include <iostream>

namespace {

// See http://www.metroid2002.com/retromodding/wiki/Collision_Index_Data_%28File_Format%29
void mapCollisionIndexData(std::fstream &file)
{
    uint32_t collision_material_count;
    file.read(reinterpret_cast<char*>(&collision_material_count), sizeof(uint32_t));
    collision_material_count = Utility::byteSwap32(collision_material_count);

    // set all walls to allow player to pass through them
    // see http://www.metroid2002.com/retromodding/wiki/Collision_Index_Data_%28File_Format%29#Collision_Materials
    bool is_wall;
    bool is_ceiling;
    bool is_solid;
    bool is_floor;
    bool is_scannable;
    std::streampos start_of_material_props = file.tellg();
    std::vector<uint32_t> new_material_props;
    new_material_props.reserve(collision_material_count);
    uint32_t i;
    uint32_t flags;

    for (i = 0; i < collision_material_count; ++i) {
        file.read(reinterpret_cast<char*>(&flags), sizeof(uint32_t));
        flags = Utility::byteSwap32(flags);
        is_wall = (flags >> 30) & 1;
        is_ceiling = (flags >> 29) & 1;
        is_solid = (flags >> 19) & 1;
        is_floor = (flags >> 31) & 1;
        is_scannable = (flags >> 27) & 1;

        if (!is_floor && is_solid && (is_wall || is_ceiling)) {
            flags ^= (1 << 19); // solid = false
            if (!is_scannable)
                flags ^= (1 << 27); // scanthrough = true
        }

        new_material_props.push_back(Utility::byteSwap32(flags));
    }

    // write new flags to file
    file.seekp(start_of_material_props);
    for (i = 0; i < collision_material_count; ++i)
        file.write(reinterpret_cast<char*>(&new_material_props[i]), sizeof(uint32_t));
}

// see http://www.metroid2002.com/retromodding/wiki/Area_Collision_%28File_Format%29
void mapCollisionSection(std::fstream &file, uint32_t root)
{
    uint32_t magic;
    file.seekg(root + 0x8); // skip to magic identifier
    file.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
    magic = Utility::byteSwap32(magic);

    if (magic != 0xDEAFBABE)
        throw std::runtime_error("Invalid area collision detected");

    // skip to Octree Size field
    file.seekg(root + 0x2C);

    uint32_t octree_size;
    file.read(reinterpret_cast<char*>(&octree_size), sizeof(uint32_t));
    octree_size = Utility::byteSwap32(octree_size);

    // skip over octree to collision index data and read it
    file.seekg(octree_size, std::ios::cur);
    mapCollisionIndexData(file);
}

} // anon namespace

namespace MREA {

// see http://www.metroid2002.com/retromodding/wiki/MREA_%28Metroid_Prime%29
void mapMrea(std::fstream &file)
{
    uint32_t magic;
    file.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
    if (Utility::byteSwap32(magic) != 0xDEADBEEF)
        throw std::runtime_error("Invalid MREA file encountered");

    // navigate to collision section of MREA
    uint32_t data_section_count;
    uint32_t collision_index;
    std::vector<uint32_t> data_section_sizes;

    file.seekg(56, std::ios::cur);
    file.read(reinterpret_cast<char*>(&data_section_count), sizeof(uint32_t));
    data_section_count = Utility::byteSwap32(data_section_count);

    file.seekg(8, std::ios::cur);
    file.read(reinterpret_cast<char*>(&collision_index), sizeof(uint32_t));
    collision_index = Utility::byteSwap32(collision_index);

    file.seekg(20, std::ios::cur);
    uint32_t size;
    data_section_sizes.reserve(data_section_count);
    for (uint32_t i = 0; i < data_section_count; ++i) {
        file.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
        data_section_sizes.push_back(Utility::byteSwap32(size));
    }

    // at the end of MREA header
    // navigate to next 32 byte boundry
    uint32_t pos = file.tellg();
    file.seekg((pos + 32u - 1u) & ~(32u - 1u));

    // navigate through data sections until we get to the collision section
    uint32_t offset = 0;
    for (uint32_t i = 0; i < collision_index; ++i)
        offset += data_section_sizes[i];

    file.seekg(offset, std::ios::cur);

    // should be at collision area of file
    uint32_t root = file.tellg();
    file.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
    if (Utility::byteSwap32(magic) != 0x01000000)
        // we got lost somehow and we no longer know where we are - abort!
        throw std::runtime_error("We got lost while navigating to collision section of an MREA file");

    mapCollisionSection(file, root);
}

} // namespace MREA