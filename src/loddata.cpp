#include "loddata.hpp"
#include "bstream.h"

#include <string>


/* UGeometry */

UGeometry::UGeometry() {

}

UGeometry::~UGeometry() {

}

void UGeometry::Deserialize(bStream::CStream* stream) {
    mVTable = stream->readUInt64();

    uint64_t vertexBufferPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint64_t indexBufferPtr = stream->readUInt64() & 0x0FFFFFFF;

    mBoneIDs = stream->readUInt64();
    m0020 = stream->readUInt64();
    m0028 = stream->readUInt64();
    m0030 = stream->readUInt64();
    m0038 = stream->readUInt64();

    stream->seek(vertexBufferPtr);
    mVertexBuffer.Deserialize(stream);

    stream->seek(indexBufferPtr);
    mIndexBuffer.Deserialize(stream);
}

void UGeometry::Serialize(bStream::CStream* stream) {

}

/* UModel */

UModel::UModel() {

}

UModel::~UModel() {
    for (UGeometry* geom : mGeometry) {
        delete geom;
    }

    mGeometry.clear();
}

void UModel::Deserialize(bStream::CStream* stream) {
    mVTable = stream->readUInt64();

    uint64_t geometryPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint16_t GeometryCount = stream->readUInt16();
    uint16_t GeometryCapacity = stream->readUInt16();
    m0014 = stream->readUInt32();

    uint64_t boundingBoxPtr = stream->readUInt64() & 0x0FFFFFFF;
    size_t streamPos = 0;

    // Read bounding boxes
    /*
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
    */

    uint64_t shaderIndicesPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint64_t boneIndicesPtr = stream->readUInt64() & 0x0FFFFFFF;

    mSkeletonBinding = stream->readUInt32();
    mRenderMaskFlags = stream->readUInt16();
    mShaderIndexCount = stream->readUInt16();
    m0038 = stream->readUInt64();

    stream->seek(geometryPtr);

    for (int i = 0; i < GeometryCount; i++) {
        uint64_t curGeometryPtr = stream->readUInt64() & 0x0FFFFFFF;

        streamPos = stream->tell();
        stream->seek(curGeometryPtr);

        UGeometry* geom = new UGeometry();
        geom->Deserialize(stream);
        mGeometry.push_back(geom);

        stream->seek(streamPos);
    }
}

void UModel::Serialize(bStream::CStream* stream) {

}

void UModel::Debug_DumpObjFile(bStream::CStream* stream) {
    uint64_t vertexCount = 1;

    for (int i = 0; i < mGeometry.size(); i++) {
        std::vector<UVertex> vertices = mGeometry[i]->GetVertexBuffer()->GetVertices();
        std::vector<uint32_t> indices = mGeometry[i]->GetIndexBuffer()->GetIndices();

        for (UVertex& v : vertices) {
            stream->writeString("v " + std::to_string(v.mPosition[0].x) + " " + std::to_string(v.mPosition[0].y) + " " + std::to_string(v.mPosition[0].z) + "\n");
        }

        stream->writeString("o geometry_" + std::to_string(i) + "\n");

        for (int i = 0; i < indices.size() / 3; i++) {
            int index = i * 3;

            stream->writeString("f " + std::to_string(indices[index] + vertexCount) + " " +
                                       std::to_string(indices[index + 1] + vertexCount) + " " +
                                       std::to_string(indices[index + 2] + vertexCount) + "\n");
        }

        vertexCount += vertices.size();
    }
}


/* ULodData */

ULodData::ULodData() {

}

ULodData::~ULodData() {
    for (UModel* model : mModels) {
        delete model;
    }

    mModels.clear();
}

void ULodData::Deserialize(bStream::CStream* stream) {
    uint64_t modelsPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint16_t modelCount = stream->readUInt16();
    uint16_t modelCapacity = stream->readUInt16();
    m000C = stream->readUInt16();

    stream->seek(modelsPtr);
    size_t streamPos = 0;

    for (int i = 0; i < modelCount; i++) {
        uint64_t curModelPtr = stream->readUInt64() & 0x0FFFFFFF;

        streamPos = stream->tell();
        stream->seek(curModelPtr);

        UModel* model = new UModel();
        model->Deserialize(stream);
        mModels.push_back(model);

        stream->seek(streamPos);
    }
}

void ULodData::Serialize(bStream::CStream* stream) {

}

void ULodData::Debug_DumpObjFile() {
    for (int i = 0; i < mModels.size(); i++) {
        bStream::CFileStream modelStream("D:\\RedDead\\model" + std::to_string(i) + ".obj", bStream::Out);

        mModels[i]->Debug_DumpObjFile(&modelStream);
    }
}
