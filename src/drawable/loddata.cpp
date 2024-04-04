#include "drawable/loddata.hpp"
#include "drawable/drawable.hpp"
#include "util/bstream.h"

#include <string>


/* CGeometry */

CGeometryData::CGeometryData() {

}

CGeometryData::~CGeometryData() {

}

void CGeometryData::Deserialize(bStream::CStream* stream) {
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

void CGeometryData::Serialize(bStream::CStream* stream) {

}

std::array<uint32_t, 8> CGeometryData::GetAttributeCounts() {
    return {
        mVertexBuffer.GetAttributeCount(static_cast<uint32_t>(EVertexAttribute::POSITION), 4),
        mVertexBuffer.GetAttributeCount(static_cast<uint32_t>(EVertexAttribute::NORMAL), 4),
        mVertexBuffer.GetAttributeCount(static_cast<uint32_t>(EVertexAttribute::TANGENT), 4),
        mVertexBuffer.GetAttributeCount(static_cast<uint32_t>(EVertexAttribute::BINORMAL), 4),
        mVertexBuffer.GetAttributeCount(static_cast<uint32_t>(EVertexAttribute::BLENDINDEX), 4),
        mVertexBuffer.GetAttributeCount(static_cast<uint32_t>(EVertexAttribute::BLENDWEIGHT), 4),
        mVertexBuffer.GetAttributeCount(static_cast<uint32_t>(EVertexAttribute::COLOR), 4),
        mVertexBuffer.GetAttributeCount(static_cast<uint32_t>(EVertexAttribute::TEXCOORD), 24),
    };
}

/* CModel */

CModelData::CModelData() {

}

CModelData::~CModelData() {
    mGeometry.clear();
}

void CModelData::Deserialize(bStream::CStream* stream) {
    mVTable = stream->readUInt64();

    uint64_t geometryPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint16_t geometryCount = stream->readUInt16();
    uint16_t geometryCapacity = stream->readUInt16();
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
    uint64_t boneTagMapPtr = stream->readUInt64() & 0x0FFFFFFF;

    uint8_t boneTagMapCount = stream->readUInt8();
    mIsSkinned = stream->readUInt8();
    uint16_t unk0032 = stream->readUInt16();

    mRenderMaskFlags = stream->readUInt16();
    mShaderIndexCount = stream->readUInt16();
    m0038 = stream->readUInt64();

    if (shaderIndicesPtr != 0) {
        stream->seek(shaderIndicesPtr);

        for (int i = 0; i < mShaderIndexCount; i++) {
            mShaderIndices.push_back(stream->readUInt16());
        }
    }

    if (boneTagMapPtr != 0) {
        stream->seek(boneTagMapPtr);

        for (int i = 0; i < boneTagMapCount; i++) {
            mBoneTagMap.push_back(stream->readUInt16());
        }
    }

    stream->seek(geometryPtr);

    for (int i = 0; i < geometryCount; i++) {
        uint64_t curGeometryPtr = stream->readUInt64() & 0x0FFFFFFF;

        streamPos = stream->tell();
        stream->seek(curGeometryPtr);

        std::shared_ptr<CGeometryData> geom = std::make_shared<CGeometryData>();
        geom->Deserialize(stream);
        mGeometry.push_back(geom);

        stream->seek(streamPos);
    }
}

void CModelData::Serialize(bStream::CStream* stream) {

}

std::shared_ptr<CModel> CModelData::GetModel() {
    std::shared_ptr<CModel> model = std::make_shared<CModel>();

    for (int i = 0; i < mGeometry.size(); i++) {
        std::shared_ptr<CGeometryData> geomData = mGeometry[i];
        std::shared_ptr<CGeometry> geom = std::make_shared<CGeometry>();

        geom->Vertices = geomData->GetVertexBuffer()->GetVertices();
        geom->Indices = geomData->GetIndexBuffer()->GetIndices();

        std::array<uint32_t, 8> attributeCounts = geomData->GetAttributeCounts();
        geom->PositionsCount = attributeCounts[0];
        geom->NormalsCount = attributeCounts[1];
        geom->TangentsCount = attributeCounts[2];
        geom->BinormalsCount = attributeCounts[3];
        geom->BlendIndicesCount = attributeCounts[4];
        geom->BlendWeightsCount = attributeCounts[5];
        geom->ColorsCount = attributeCounts[6];
        geom->TexCoordsCount = attributeCounts[7];

        geom->ShaderIndex = mShaderIndices[i];

        model->Geometries.push_back(geom);
    }

    return model;
}

void CModelData::Debug_DumpObjFile(bStream::CStream* stream) {
    uint64_t vertexCount = 1;

    for (int i = 0; i < mGeometry.size(); i++) {
        shared_vector<CVertex> vertices = mGeometry[i]->GetVertexBuffer()->GetVertices();
        std::vector<uint32_t> indices = mGeometry[i]->GetIndexBuffer()->GetIndices();

        for (std::shared_ptr<CVertex> v : vertices) {
            stream->writeString("v " + std::to_string(v->Position[0].x) + " " + std::to_string(v->Position[0].y) + " " + std::to_string(v->Position[0].z) + "\n");
            stream->writeString("vt " + std::to_string(v->TexCoord[0].x) + " " + std::to_string(v->TexCoord[0].y) + "\n");
            stream->writeString("vn " + std::to_string(v->Normal[0].x) + " " + std::to_string(v->Normal[0].y) + " " + std::to_string(v->Normal[0].z) + "\n");
        }

        stream->writeString("o geometry_" + std::to_string(i) + "\n");

        for (int i = 0; i < indices.size() / 3; i++) {
            int index = i * 3;

            std::string v1 = std::to_string(indices[index] + vertexCount);
            std::string v2 = std::to_string(indices[index + 1] + vertexCount);
            std::string v3 = std::to_string(indices[index + 2] + vertexCount);

            stream->writeString("f " + v1 + "/" + v1 + "/" + v1 + " " +
                                       v2 + "/" + v2 + "/" + v2 + " " +
                                       v3 + "/" + v3 + "/" + v3 + "\n");
        }

        vertexCount += vertices.size();
    }
}


/* CLodData */

CLodData::CLodData() {

}

CLodData::~CLodData() {
    mModels.clear();
}

void CLodData::Deserialize(bStream::CStream* stream) {
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

        std::shared_ptr<CModelData> model = std::make_shared<CModelData>();
        model->Deserialize(stream);
        mModels.push_back(model);

        stream->seek(streamPos);
    }
}

void CLodData::Serialize(bStream::CStream* stream, uint8_t lodIndex) {

}

std::shared_ptr<CLod> CLodData::GetLod() {
    std::shared_ptr<CLod> lod = std::make_shared<CLod>();

    for (std::shared_ptr<CModelData> modelData : mModels) {
        lod->Models.push_back(modelData->GetModel());
    }

    return lod;
}

void CLodData::Debug_DumpObjFile() {
    for (int i = 0; i < mModels.size(); i++) {
        bStream::CFileStream modelStream("D:\\RedDead\\model" + std::to_string(i) + ".obj", bStream::Out);

        mModels[i]->Debug_DumpObjFile(&modelStream);
    }
}
