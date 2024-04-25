// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/crypto/crc.h

#pragma once

#include "types.h"

extern const uint64_t CRC64Table[256];

inline uint64_t CRC64(const void* data, size_t length, uint64_t crc = 0)
{
    const uint8_t* data8 = static_cast<const uint8_t*>(data);

    crc = ~crc;

    for (size_t i = 0; i < length; i++)
        crc = (crc >> 8) ^ CRC64Table[(crc ^ data8[i]) & 0xFF];

    return ~crc;
}
