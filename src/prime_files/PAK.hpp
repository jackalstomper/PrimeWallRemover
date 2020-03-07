#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include "../Common.hpp"

namespace PAK {

struct Header
{
    Common::BigEndian<uint16_t> majorVersion;
    Common::BigEndian<uint16_t> minorVersion;
    Common::BigEndian<uint32_t> unused;
};

struct NamedResource
{
    std::string assetType;
    Common::asset_id assetID;
    std::string name;
};

struct Resource
{
    Common::BigEndian<uint32_t> isCompressed;
    std::string assetType;
    Common::asset_id assetID;
    Common::BigEndian<uint32_t> size;
    Common::BigEndian<uint32_t> offset;
};

struct Index
{
    Header header;
    std::vector<NamedResource> namedResources;
    std::vector<Resource> resources;

    void read(std::fstream& file);
};

} // namespace PAK
