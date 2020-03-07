#include "PAK.hpp"

namespace PAK {

void Index::read(std::fstream& file) {
    file.seekg(0);
    header.majorVersion.read(file);
    header.minorVersion.read(file);
    header.unused.read(file);

    Common::BigEndian<uint32_t> namedCount;
    namedCount.read(file);
    char buff[4];
    for (uint32_t i = 0; i < namedCount(); ++i) {
        NamedResource namedResource;
        file.read(buff, 4);
        namedResource.assetType = std::string(buff, 4);
        namedResource.assetID.read(file);
        Common::BigEndian<uint32_t> nameLen;
        nameLen.read(file);
        std::vector<char> nameBuff(nameLen());
        file.read(nameBuff.data(), nameLen());
        namedResource.name = std::string(nameBuff.data(), nameLen());
        namedResources.push_back(namedResource);
    }

    Common::BigEndian<uint32_t> resourceCount;
    resourceCount.read(file);
    for (uint32_t i = 0; i < resourceCount(); ++i) {
        Resource resource;
        resource.isCompressed.read(file);
        file.read(buff, 4);
        resource.assetType = std::string(buff, 4);
        resource.assetID.read(file);
        resource.size.read(file);
        resource.offset.read(file);
        resources.push_back(resource);
    }
}

} // namespace PAK
