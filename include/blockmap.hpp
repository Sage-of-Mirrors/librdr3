#pragma once

#include "types.h"

namespace bStream {
    class CStream;
}

class UBlockMap {
    uint32_t m0000;             // 0x00
    uint32_t m0004;             // 0x04
    uint8_t mVirtualPageCount;  // 0x08
    uint8_t mPhysicalPageCount; // 0x09
    uint16_t m000A;             // 0x0A
    uint32_t m000C;             // 0x0c

    uint32_t m0010; // 0x10
    uint32_t m0014; // 0x14
    uint32_t m0018; // 0x18
    uint32_t m001C; // 0x1C

public:
    UBlockMap();
    virtual ~UBlockMap();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);
};
