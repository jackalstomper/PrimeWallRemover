#include "nod/Util.hpp"
#include <cstring>

namespace Common {

uint16_t bswap(uint16_t v) {
    return nod::bswap16(v);
}

uint32_t bswap(uint32_t v) {
    return nod::bswap32(v);
}

uint64_t bswap(uint64_t v) {
    return nod::bswap64(v);
}

float bswap(float v) {
    static char buff[4];
    memcpy(buff, &v, 4);
    char* vp = reinterpret_cast<char*>(&v);
    vp[0] = buff[3];
    vp[1] = buff[2];
    vp[2] = buff[1];
    vp[3] = buff[0];
    return v;
}

} // namespace Utility