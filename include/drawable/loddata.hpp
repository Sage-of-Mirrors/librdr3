#pragma once

#include "types.h"
#include "vertexdata.hpp"

#include "util/math.hpp"

#include <vector>

struct CLod;
struct CModel;
struct CGeometry;

class CGeometryData {
    uint64_t mVTable;

    CVertexBuffer mVertexBuffer;
    CIndexBuffer mIndexBuffer;

    uint64_t mBoneIDs;
    uint64_t m0020;
    uint64_t m0028;
    uint64_t m0030;
    uint64_t m0038;

public:
    CGeometryData();
    virtual ~CGeometryData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    CVertexBuffer* GetVertexBuffer() { return &mVertexBuffer; }
    CIndexBuffer* GetIndexBuffer() { return &mIndexBuffer; }

    std::array<uint32_t, 8> GetAttributeCounts();
};

class CModelData {
    uint64_t mVTable;

    shared_vector<CGeometryData> mGeometry;
    uint32_t m0014;

    std::vector<Vector4> mBoundingBoxes;
    std::vector<uint16_t> mShaderIndices;

    std::vector<uint16_t> mBoneTagMap;
    bool mIsSkinned;

    uint16_t mRenderMaskFlags;
    uint16_t mShaderIndexCount;

    uint64_t m0038;

public:
    CModelData();
    virtual ~CModelData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    std::shared_ptr<CModel> GetModel();

    void Debug_DumpObjFile(bStream::CStream* stream);
};

class CLodData {
    shared_vector<CModelData> mModels;
    uint32_t m000C;

public:
    CLodData();
    virtual ~CLodData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream, uint8_t lodIndex);

    std::shared_ptr<CLod> GetLod();

    void Debug_DumpObjFile();
};
