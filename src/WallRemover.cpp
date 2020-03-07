#include "WallRemover.hpp"
#include "prime_files/MREA.hpp"

namespace PWR {

void removeWalls(std::vector<char>& buff) {
    char* bp = buff.data();
    MREA::Header header;
    header.magic.read(&bp);
    if (header.magic() != 0xDEADBEEF)
        throw std::runtime_error("Failed to parse MREA header correctly");

    header.version.read(&bp);
    for (int i = 0; i < 12; ++i)
        header.areaTransform[i].read(&bp);

    header.worldModelCount.read(&bp);
    header.dataSectionCount.read(&bp);
    header.worldGeometrySection.read(&bp);
    header.scriptLayersSection.read(&bp);
    header.collisionSection.read(&bp);
    header.unknownSection.read(&bp);
    header.lightsSection.read(&bp);
    header.visibilityTreeSection.read(&bp);
    header.pathSection.read(&bp);
    header.areaOctreeSection.read(&bp);

    std::vector<Common::BigEndian<uint32_t>> sectionSizes;
    for (uint32_t i = 0; i < header.dataSectionCount(); i++) {
        Common::BigEndian<uint32_t> size;
        size.read(&bp);
        sectionSizes.push_back(size);
    }

    // The header ends at the next 32 byte boundry
    uintptr_t bpa = reinterpret_cast<uintptr_t>(bp);
    if (bpa % 32 != 0)
        bpa += 32 - bpa % 32;

    bp = reinterpret_cast<char*>(bpa);
    for (uint32_t i = 0; i < header.collisionSection(); i++)
        bp += sectionSizes[i]();

    MREA::AreaCollision areaCollision;
    areaCollision.unknown.read(&bp);
    areaCollision.sectionSize.read(&bp);
    areaCollision.magic.read(&bp);
    areaCollision.version.read(&bp);
    bp += 0x18; // skip AAbox
    areaCollision.octreeNodeType.read(&bp);
    areaCollision.octreeSize.read(&bp);

    if (areaCollision.magic() != 0xDEAFBABE)
        throw std::runtime_error("Something has broken!");

    // Skip the collision octree, we don't need it
    bp += areaCollision.octreeSize();

    Common::BigEndian<uint32_t> materialCount;
    materialCount.read(&bp);

    union
    {
        MREA::CollisionMaterialFlags flags;
        uint32_t _dummy = 0;
    };

    uint32_t* ip;
    for (uint32_t i = 0; i < materialCount(); ++i) {
        ip = reinterpret_cast<uint32_t*>(bp);
        _dummy = Common::bswap(*ip);
        
        if (!flags.floor && flags.solid && (flags.wall || flags.ceiling)) {
            flags.solid = false;
            flags.scanThru = true;
        }

        *ip = Common::bswap(_dummy);
        bp += sizeof(uint32_t);
    }
}

} // namespace PWR