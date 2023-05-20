#pragma once

#include "types.h"

#include <string>

const uint32_t LOD_MAX = 4;

class ULodData;
namespace bStream {
    class CStream;
}

/* The LOD levels that drawables can define. */
enum class EDrawableLod : uint8_t {
    LOD_HIGH = 0,
    LOD_MED,
    LOD_LOW,
    LOD_VLOW,

    LOD_MAX
};

class UDrawable {
    uint64_t mVTable;            // 0x00

    class UBlockMap* mBlockMap;        // 0x08
    class UShaderGroup* mShaderGroup;  // 0x10
    uint64_t mSkeletonData;      // 0x18

    UVector4 mBoundingSphere; // 0x20, w is radius
    UVector4 mBoundingBoxMin; // 0x30, w has no meaning
    UVector4 mBoundingBoxMax; // 0x40, w has no meaning

    ULodData* mLodData[LOD_MAX];   // 0x50, 0x58, 0x60, 0x68
    float mLodDistances[LOD_MAX];  // 0x70, 0x74, 0x78, 0x7C
    uint32_t mLodFlags[LOD_MAX];   // 0x80, 0x84, 0x88, 0x8C

    class UJointLimitData* mJointLimitData; // 0x90
    uint64_t mPadding1;   // 0x98

    uint16_t mExpressionCount;  // 0xA0
    uint16_t mPadding2;         // 0xA2
    uint32_t mExpressions;      // 0xA4

    std::string mName;         // 0xA8
    uint64_t m00B0;         // 0xB0
    uint64_t mBoundPointer; // 0xB8
    uint64_t mSamplers;     // 0xC0
    uint64_t mPadding3;     // 0xC8

public:
    UDrawable();
    virtual ~UDrawable();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);
};
