#pragma once

#include "types.h"
#include "util/math.hpp"

#include <array>
#include <vector>

/* The formats that vertex data can be stored in. */
enum class EVertexFormat : uint8_t {
    NONE = 0,
    RGBA32_FLOAT = 2,
    RGB32_FLOAT = 6,
    RGBA16_FLOAT = 10,
    RG32 = 16,
    RGB10A2 = 24,
    RGBA8_UNORM = 28,
    RGBA8_UINT = 30,
    RG16_FLOAT = 34
};

/* The various vertex attributes that a model can have. */
enum class EVertexAttribute : uint8_t {
    POSITION = 0,
    POSITION1,
    POSITION2,
    POSITION3,
    NORMAL,
    NORMAL1,
    NORMAL2,
    NORMAL3,
    TANGENT,
    TANGENT1,
    TANGENT2,
    TANGENT3,
    BINORMAL,
    BINORMAL1,
    BINORMAL2,
    BINORMAL3,
    BLENDWEIGHT,
    BLENDWEIGHT1,
    BLENDWEIGHT2,
    BLENDWEIGHT3,
    BLENDINDEX,
    BLENDINDEX1,
    BLENDINDEX2,
    BLENDINDEX3,
    COLOR,
    COLOR1,
    COLOR2,
    COLOR3,
    TEXCOORD,
    TEXCOORD1,
    TEXCOORD2,
    TEXCOORD3,
    TEXCOORD4,
    TEXCOORD5,
    TEXCOORD6,
    TEXCOORD7,
    TEXCOORD8,
    TEXCOORD9,
    TEXCOORD10,
    TEXCOORD11,
    TEXCOORD12,
    TEXCOORD13,
    TEXCOORD14,
    TEXCOORD15,
    TEXCOORD16,
    TEXCOORD17,
    TEXCOORD18,
    TEXCOORD19,
    TEXCOORD20,
    TEXCOORD21,
    TEXCOORD22,
    TEXCOORD23,

    MAX
};

struct CVertex {
    std::array<Vector3, 4> Position;
    std::array<Vector3, 4> Normal;
    std::array<Vector3, 4> Tangent;
    std::array<Vector3, 4> Binormal;
    std::array<Vector4, 4> Color;
    std::array<Vector4, 4> BlendWeight;
    std::array<Vector4, 4> BlendIndex;
    std::array<Vector2, 24> TexCoord;
};

struct CVertexFormatData {
    uint32_t mOffsets[(uint32_t)EVertexAttribute::MAX];    // 0x0000
    uint8_t mSizes[(uint32_t)EVertexAttribute::MAX];       // 0x00D0
    EVertexFormat mTypes[(uint32_t)EVertexAttribute::MAX]; // 0x0104

    uint32_t mData; // 0x0138
    uint32_t m013C; // 0x013C

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CMemoryStream* stream);
};

class CVertexBuffer {
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
    CVertexFormatData mVertexFormat;  // 0x38

    std::array<float, 4> ReadVertexComponent(bStream::CStream& stream, EVertexFormat format);
    void WriteVertexComponent(bStream::CStream* stream, EVertexFormat format, std::array<float, 4> data);

    uint16_t CalculateVertexStride(std::vector<EVertexFormat> formats);

public:
    CVertexBuffer();
    virtual ~CVertexBuffer();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CMemoryStream* stream);

    shared_vector<CVertex> GetVertices();
    void SetVertices(shared_vector<CVertex>& vertices, std::vector<EVertexFormat> formats);
    
    uint32_t GetAttributeCount(uint32_t startingIndex, uint32_t max);
};

class CIndexBuffer {
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
    CIndexBuffer();
    virtual ~CIndexBuffer();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    std::vector<uint32_t> GetIndices();
};
