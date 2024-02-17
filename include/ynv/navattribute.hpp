#pragma once

#include "types.h"
#include "math.hpp"

// 0x10
//struct UNavAttribUnk {
//    uint32_t m0000; // 0x00
//    uint32_t m0004; // 0x04
//    uint32_t m0008; // 0x08
//    uint32_t m000C; // 0x0C
//};

// 0x10
struct UNavAttribDescriptor {
    uint64_t mDataOffset; // 0x00
    uint32_t mCount;      // 0x08
    uint32_t m000C;       // 0x0C
};

// 0x30
struct UNavAttribute {
    float m0000;                      // 0x00, is this really a float though?
    uint32_t m0004;                   // 0x04
    
    uint32_t mCount;                  // 0x08
    uint32_t m000C;                   // 0x0C

    UNavAttribDescriptor mDescriptor;          // 0x10
    UNavAttribDescriptor mSecondaryDescriptor; // 0x18

    uint32_t m0020;                   // 0x20
    uint32_t m0024;                   // 0x24
    uint32_t m0028;                   // 0x28
    uint32_t m002C;                   // 0x2C

    void Serialize(bStream::CStream* stream);
    void Deserialize(bStream::CStream* stream);
};
