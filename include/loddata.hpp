#pragma once

#include "types.h"
#include "vertexdata.hpp"

#include <vector>

namespace bStream {
    class CStream;
}

class UGeometry {
    uint64_t mVTable;

    UVertexBuffer mVertexBuffer;
    UIndexBuffer mIndexBuffer;

    uint64_t mBoneIDs;
    uint64_t m0020;
    uint64_t m0028;
    uint64_t m0030;
    uint64_t m0038;

public:
    UGeometry();
    virtual ~UGeometry();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    UVertexBuffer* GetVertexBuffer() { return &mVertexBuffer; }
    UIndexBuffer* GetIndexBuffer() { return &mIndexBuffer; }
};

class UModel {
    uint64_t mVTable;

    std::vector<UGeometry*> mGeometry;
    uint32_t m0014;

    std::vector<UVector4> mBoundingBoxes;
    std::vector<uint16_t> mShaderIndices;

    uint16_t mBoneIndices;
    uint32_t mSkeletonBinding;

    uint16_t mRenderMaskFlags;
    uint16_t mShaderIndexCount;

    uint64_t m0038;

public:
    UModel();
    virtual ~UModel();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    void Debug_DumpObjFile(bStream::CStream* stream);
};

class ULodData {
    std::vector<UModel*> mModels;
    uint32_t m000C;

public:
    ULodData();
    virtual ~ULodData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    void Debug_DumpObjFile();
};
