#ifndef LIBDRAWABLE_VERTEXDATA_H
#define LIBDRAWABLE_VERTEXDATA_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct UStream UStream;

    /* Contains all of the data making up a single vertex. */
    typedef struct UVertex {
        UVector3 mPosition[4];
        UVector3 mNormal[4];
        UVector3 mTangent[4];
        UVector3 mBinormal[4];
        UVector4 mColor[4];
        UVector4 mBlendWeight[4];
        UVector4 mBlendIndex[4];
        UVector2 mTexCoord[24];
    } UVertex;

    typedef struct UVertexFormat {
        uint32_t mOffsets[52]; // 0x0000
        uint8_t mSizes[52];    // 0x00D0
        EVertexFormat mTypes[52];    // 0x0104

        uint32_t mData; // 0x0138
        uint32_t m013C; // 0x013C
    } UVertexFormat;

    typedef struct UVertexBuffer {
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
        UVertexFormat* mVertexFormat; // 0x38
    } UVertexBuffer;

    typedef struct UIndexBuffer {
        uint64_t mVTable; // 0x00

        uint32_t mIndexCount;  // 0x08
        uint16_t mIndexStride; // 0x0C
        uint16_t m000E;        // 0x0E

        uint32_t mFlags; // 0x10
        uint32_t m0014;  // 0x14

        uint16_t* mIndexData; // 0x18
        uint64_t m0020;       // 0x20
        uint64_t m0028;       // 0x28

        uint64_t mShaderResourceView; // 0x30
        uint64_t m0038;       // 0x38

        uint64_t m0040; // 0x40
        uint64_t m0048;            // 0x48
    } UIndexBuffer;

    void DeserializeVertexBuffer(UStream* stream, UVertexBuffer* dst);
    void SerializeVertexBuffer(UStream* stream, UVertexBuffer* src);

    void DeserializeIndexBuffer(UStream* stream, UIndexBuffer* dst);
    void SerializeIndexBuffer(UStream* stream, UIndexBuffer* src);

    void DeserializeVertexFormat(UStream* stream, UVertexFormat* dst);
    void SerializeVertexFormat(UStream* stream, UVertexFormat* src);

    UVertex* GetVertices(const UVertexBuffer* buf);

#ifdef __cplusplus
}
#endif

#endif