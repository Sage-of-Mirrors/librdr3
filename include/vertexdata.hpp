#pragma once

#include "types.h"
#include "math.hpp"

#include <vector>

namespace bStream {
    class CStream;
}

/* The formats that vertex data can be stored in. */
enum class EVertexFormat : uint8_t {
    VFT_NONE = 0,
    VFT_RGBA32_FLOAT = 2,
    VFT_RGB32_FLOAT = 6,
    VFT_RGBA16_FLOAT = 10,
    VFT_RG32 = 16,
    VFT_RGB10A2 = 24,
    VFT_RGBA8_UNORM = 28,
    VFT_RGBA8_UINT = 30,
    VFT_RG16_FLOAT = 34
};

/* The various vertex attributes that a model can have. */
enum class EVertexAttribute : uint8_t {
    VAT_POSITION = 0,
    VAT_POSITION1,
    VAT_POSITION2,
    VAT_POSITION3,
    VAT_NORMAL,
    VAT_NORMAL1,
    VAT_NORMAL2,
    VAT_NORMAL3,
    VAT_TANGENT,
    VAT_TANGENT1,
    VAT_TANGENT2,
    VAT_TANGENT3,
    VAT_BINORMAL,
    VAT_BINORMAL1,
    VAT_BINORMAL2,
    VAT_BINORMAL3,
    VAT_BLENDWEIGHT,
    VAT_BLENDWEIGHT1,
    VAT_BLENDWEIGHT2,
    VAT_BLENDWEIGHT3,
    VAT_BLENDINDEX,
    VAT_BLENDINDEX1,
    VAT_BLENDINDEX2,
    VAT_BLENDINDEX3,
    VAT_COLOR,
    VAT_COLOR1,
    VAT_COLOR2,
    VAT_COLOR3,
    VAT_TEXCOORD,
    VAT_TEXCOORD1,
    VAT_TEXCOORD2,
    VAT_TEXCOORD3,
    VAT_TEXCOORD4,
    VAT_TEXCOORD5,
    VAT_TEXCOORD6,
    VAT_TEXCOORD7,
    VAT_TEXCOORD8,
    VAT_TEXCOORD9,
    VAT_TEXCOORD10,
    VAT_TEXCOORD11,
    VAT_TEXCOORD12,
    VAT_TEXCOORD13,
    VAT_TEXCOORD14,
    VAT_TEXCOORD15,
    VAT_TEXCOORD16,
    VAT_TEXCOORD17,
    VAT_TEXCOORD18,
    VAT_TEXCOORD19,
    VAT_TEXCOORD20,
    VAT_TEXCOORD21,
    VAT_TEXCOORD22,
    VAT_TEXCOORD23,

    VAT_MAX
};

struct UVertex {
    UVector3 mPosition[4];
    UVector3 mNormal[4];
    UVector3 mTangent[4];
    UVector3 mBinormal[4];
    UVector4 mColor[4];
    UVector4 mBlendWeight[4];
    UVector4 mBlendIndex[4];
    UVector2 mTexCoord[24];
};

struct UVertexFormat {
    uint32_t mOffsets[(uint32_t)EVertexAttribute::VAT_MAX];    // 0x0000
    uint8_t mSizes[(uint32_t)EVertexAttribute::VAT_MAX];       // 0x00D0
    EVertexFormat mTypes[(uint32_t)EVertexAttribute::VAT_MAX]; // 0x0104

    uint32_t mData; // 0x0138
    uint32_t m013C; // 0x013C

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);
};

class UVertexBuffer {
    uint64_t mVTable; // 0x00

    uint32_t mVertexCount;  // 0x08
    uint16_t mVertexStride; // 0x0C
    uint16_t m000E;         // 0x0E

    uint32_t mBindFlags; // 0x10
    uint32_t m0014;      // 0x14

    uint8_t* mVertexData; // 0x18
    uint64_t m0020;       // 0x20
    uint64_t m0028;       // 0x28

    uint64_t mShaderResourceView; // 0x30
    UVertexFormat mVertexFormat;  // 0x38

    float ReadVertexComponent(bStream::CStream& stream, EVertexFormat format);

public:
    UVertexBuffer();
    virtual ~UVertexBuffer();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    std::vector<UVertex> GetVertices();
};

class UIndexBuffer {
    uint64_t mVTable; // 0x00

    uint32_t mIndexCount;  // 0x08
    uint16_t mIndexStride; // 0x0C
    uint16_t m000E;        // 0x0E

    uint32_t mFlags; // 0x10
    uint32_t m0014;  // 0x14

    uint8_t* mIndexData; // 0x18
    uint64_t m0020;      // 0x20
    uint64_t m0028;      // 0x28

    uint64_t mShaderResourceView; // 0x30
    uint64_t m0038;               // 0x38

    uint64_t m0040; // 0x40
    uint64_t m0048; // 0x48

public:
    UIndexBuffer();
    virtual ~UIndexBuffer();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    std::vector<uint32_t> GetIndices();
};
