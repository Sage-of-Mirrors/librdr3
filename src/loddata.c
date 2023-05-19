#include "loddata.h"
#include "vertexdata.h"
#include "stream.h"

void DeserializeLodData(UStream* stream, ULodData* dst) {
    uint64_t modelsPtr = UStreamReadU64(stream);
    dst->mModelCount = UStreamReadU16(stream);
    dst->mModelCapacity = UStreamReadU16(stream);
    dst->m000C = UStreamReadU32(stream);

    dst->mModels = (UModel*)malloc(sizeof(UModel) * dst->mModelCount);

    UStreamSeek(stream, modelsPtr - 0x50000000, 0);
    size_t streamPos = 0;

    for (int i = 0; i < dst->mModelCount; i++) {
        uint64_t curModelPtr = UStreamReadU64(stream);

        streamPos = stream->position;
        UStreamSeek(stream, curModelPtr - 0x50000000, 0);
        DeserializeModelData(stream, &dst->mModels[i]);
        stream->position = streamPos;
    }
}

void SerializeLodData(UStream* stream, ULodData* src) {

}

void DeserializeModelData(UStream* stream, UModel* dst) {
    size_t streamPos = 0;
    dst->mVTable = UStreamReadU64(stream);

    uint64_t geometryPtr = UStreamReadU64(stream);
    dst->mGeometryCount = UStreamReadU16(stream);
    dst->mGeometryCapacity = UStreamReadU16(stream);
    dst->m0014 = UStreamReadU32(stream);

    dst->mGeometry = (UGeometry*)malloc(sizeof(UGeometry) * dst->mGeometryCount);

    uint64_t boundingBoxPtr = UStreamReadU64(stream);

    // Read bounding boxes
    streamPos = stream->position;
    UStreamSeek(stream, boundingBoxPtr - 0x50000000, 0);

    dst->mBoundingBox[0].x = UStreamReadFloat(stream);
    dst->mBoundingBox[0].z = UStreamReadFloat(stream);
    dst->mBoundingBox[0].y = UStreamReadFloat(stream);
    dst->mBoundingBox[0].w = UStreamReadFloat(stream);

    dst->mBoundingBox[1].x = UStreamReadFloat(stream);
    dst->mBoundingBox[1].z = UStreamReadFloat(stream);
    dst->mBoundingBox[1].y = UStreamReadFloat(stream);
    dst->mBoundingBox[1].w = UStreamReadFloat(stream);

    stream->position = streamPos;

    uint64_t shaderIndicesPtr = UStreamReadU64(stream);
    uint64_t boneIndicesPtr = UStreamReadU64(stream);

    dst->mSkeletonBinding = UStreamReadU32(stream);
    dst->mRenderMaskFlags = UStreamReadU16(stream);
    dst->mShaderIndexCount = UStreamReadU16(stream);
    dst->m0038 = UStreamReadU64(stream);

    UStreamSeek(stream, geometryPtr - 0x50000000, 0);

    for (int i = 0; i < dst->mGeometryCount; i++) {
        uint64_t curGeometryPtr = UStreamReadU64(stream);

        streamPos = stream->position;
        UStreamSeek(stream, curGeometryPtr - 0x50000000, 0);
        DeserializeGeometryData(stream, &dst->mGeometry[i]);
        stream->position = streamPos;
    }
}

void SerializeModelData(UStream* stream, UModel* src) {

}

void DeserializeGeometryData(UStream* stream, UGeometry* dst) {
    size_t streamPos = 0;
    dst->mVTable = UStreamReadU64(stream);

    uint64_t vertexBufferPtr = UStreamReadU64(stream);
    uint64_t indexBufferPtr = UStreamReadU64(stream);

    dst->mVertexBuffer = (UVertexBuffer*)malloc(sizeof(UVertexBuffer));
    dst->mIndexBuffer = (UIndexBuffer*)malloc(sizeof(UIndexBuffer));

    dst->mBoneIDs = UStreamReadU64(stream);
    dst->m0020 = UStreamReadU64(stream);
    dst->m0028 = UStreamReadU64(stream);
    dst->m0030 = UStreamReadU64(stream);
    dst->m0038 = UStreamReadU64(stream);

    UStreamSeek(stream, vertexBufferPtr - 0x50000000, 0);
    DeserializeVertexBuffer(stream, dst->mVertexBuffer);
}

void SerializeGeometryData(UStream* stream, UGeometry* src) {

}
