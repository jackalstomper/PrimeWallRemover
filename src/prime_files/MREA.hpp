#pragma once

#include "../Common.hpp"
#include "PAK.hpp"
#include <vector>
#include <fstream>

namespace MREA {

struct Header
{
    Common::BigEndian<uint32_t> magic;
    Common::BigEndian<uint32_t> version;
    Common::BigEndian<float> areaTransform[12];
    Common::BigEndian<uint32_t> worldModelCount;
    Common::BigEndian<uint32_t> dataSectionCount;
    Common::BigEndian<uint32_t> worldGeometrySection;
    Common::BigEndian<uint32_t> scriptLayersSection;
    Common::BigEndian<uint32_t> collisionSection;
    Common::BigEndian<uint32_t> unknownSection;
    Common::BigEndian<uint32_t> lightsSection;
    Common::BigEndian<uint32_t> visibilityTreeSection;
    Common::BigEndian<uint32_t> pathSection;
    Common::BigEndian<uint32_t> areaOctreeSection;
};

struct CollisionMaterialFlags
{
    bool unknown : 1;
    bool stone : 1;
    bool metal : 1;
    bool grass : 1;
    bool ice : 1;
    bool pillar : 1;
    bool metalGrating : 1;
    bool phazon : 1;
    bool dirt : 1;
    bool lava : 1;
    bool unknown1 : 1;
    bool snow : 1;
    bool slowMud : 1;
    bool halfpipe : 1;
    bool mud : 1;
    bool glass : 1;
    bool shield : 1;
    bool sand : 1;
    bool shootThru : 1;
    bool solid : 1;
    bool unknown2 : 1;
    bool cameraThru : 1;
    bool wood : 1;
    bool organic : 1;
    bool unknown3 : 1;
    bool flippedTri : 1;
    bool seeThru : 1;
    bool scanThru : 1;
    bool aiWalkThru : 1;
    bool ceiling : 1;
    bool wall : 1;
    bool floor : 1;
};
static_assert(sizeof(CollisionMaterialFlags) == 4, "CollisionMaterialFlags is wrong size");
static_assert(std::is_pod<CollisionMaterialFlags>::value, "CollisionMaterialFlags isn't POD");

struct AreaCollision
{
    Common::BigEndian<uint32_t> unknown;
    Common::BigEndian<uint32_t> sectionSize; // Size of the collision section (not including these first two values).
    Common::BigEndian<uint32_t> magic;
    Common::BigEndian<uint32_t> version;
    Common::BigEndian<uint32_t> octreeNodeType;
    Common::BigEndian<uint32_t> octreeSize;
};

} // namespace MREA