#ifdef _MSC_VER
#   include <intrin.h>
#endif

#include "utility.hpp"

namespace Utility {

uint32_t byteSwap32(uint32_t value)
{
#ifdef _MSC_VER
    return _byteswap_ulong(value);
#else
    return __builtin_bswap32(value);
#endif
}

}