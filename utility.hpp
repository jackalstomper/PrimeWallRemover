#pragma once

#include <fstream>

#ifdef _MSC_VER
#   include <stdint.h>
#endif

namespace Utility {

uint32_t byteSwap32(uint32_t value);

}