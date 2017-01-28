#include "area_collision.hpp"
#include "utility.hpp"
#include <vector>
#include <iostream>

namespace {

// See http://www.metroid2002.com/retromodding/wiki/Collision_Index_Data_%28File_Format%29
void mapCollisionIndexData(std::fstream &file)
{
    uint32_t collision_material_count;
    uint32_t vertex_index_count;
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
            std::cout << "Setting material " << i << " to be passthrough!\n";
        }

        new_material_props.push_back(Utility::byteSwap32(flags));
    }

    // write new flags to file
    file.seekp(start_of_material_props);
    for (i = 0; i < collision_material_count; ++i)
        file.write(reinterpret_cast<char*>(&new_material_props[i]), sizeof(uint32_t));
}

} // anon namespace

namespace MREA {

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

} // namespace MREA