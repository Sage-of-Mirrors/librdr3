#ifndef LIBDRAWABLE_LODDATA_H
#define LIBDRAWABLE_LODDATA_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct UStream UStream;
    typedef struct UVertexBuffer UVertexBuffer;
    typedef struct UIndexBuffer UIndexBuffer;

    typedef struct UGeometry {
        uint64_t mVTable; // 0x00

        UVertexBuffer* mVertexBuffer; // 0x08
        UIndexBuffer* mIndexBuffer;   // 0x10

        uint64_t mBoneIDs; // 0x18
        uint64_t m0020;    // 0x20
        uint64_t m0028;    // 0x28
        uint64_t m0030;    // 0x30
        uint64_t m0038;    // 0x38
    } UGeometry;

    typedef struct UModel {
        uint64_t mVTable; // 0x00

        UGeometry* mGeometry;       // 0x08
        uint16_t mGeometryCount;    // 0x10
        uint16_t mGeometryCapacity; // 0x12
        uint32_t m0014;             // 0x14

        UVector4 mBoundingBox[2]; // 0x18

        uint16_t* mShaderIndices; // 0x20

        uint16_t mBoneIndices;     // 0x28
        uint32_t mSkeletonBinding; // 0x30

        uint16_t mRenderMaskFlags;  // 0x34
        uint16_t mShaderIndexCount; // 0x36

        uint64_t m0038; // 0x38
    } UModel;

    typedef struct ULodData {
        UModel* mModels;         // 0x00
        uint16_t mModelCount;    // 0x08
        uint16_t mModelCapacity; // 0x0A
        uint32_t m000C;          // 0x0C
    } ULodData;

    void DeserializeLodData(UStream* stream, ULodData* dst);
    void SerializeLodData(UStream* stream, ULodData* src);

    void DeserializeModelData(UStream* stream, UModel* dst);
    void SerializeModelData(UStream* stream, UModel* src);

    void DeserializeGeometryData(UStream* stream, UGeometry* dst);
    void SerializeGeometryData(UStream* stream, UGeometry* src);

#ifdef __cplusplus
}
#endif

#endif