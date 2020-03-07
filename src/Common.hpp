#pragma once

#include <cstdint>
#include <fstream>
#include <cstring>

namespace Common {

uint16_t bswap(uint16_t val);
uint32_t bswap(uint32_t val);
uint64_t bswap(uint64_t val);
float bswap(float val);

template<typename T>
struct BigEndian
{
    static_assert(std::is_pod<T>::value, "T must be POD");

    T m_val; // our value in BE encoding

    // Returns our value in LE
    T operator()() const { return bswap(m_val); }

    // assigns a LE number to our value
    BigEndian& operator=(T o) {
        m_val = bswap(o);
        return *this;
    }

    BigEndian& operator=(const BigEndian<T>& o) {
        m_val = o.m_val;
        return *this;
    }

    BigEndian& read(std::fstream& file) {
        file.read(reinterpret_cast<char*>(&m_val), sizeof(T));
        return *this;
    }

    BigEndian& read(char** ptr) {
        T* p = reinterpret_cast<T*>(*ptr);
        m_val = *p;
        (*ptr) += sizeof(T);
        return *this;
    }

    bool operator==(const BigEndian<T>& o) const {
        return m_val == o.m_val;
    }
};

using asset_id = BigEndian<uint32_t>;

} // namespace Utility