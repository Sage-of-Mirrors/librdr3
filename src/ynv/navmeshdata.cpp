#include "ynv/navmeshdata.hpp"
#include "ynv/navattribute.hpp"

#include <bstream.h>

UNavmeshData::UNavmeshData() {

}

UNavmeshData::~UNavmeshData() {

}

void UNavmeshData::ReadVertexData(bStream::CStream* stream, UNavAttribute* attribute) {
    size_t streamPos = stream->tell();
    stream->seek(attribute->mDescriptorsOffset);

    for (uint32_t i = 0; i < attribute->mPoolCount; i++) {
        uint64_t dataOffset = stream->readUInt64() & 0x0FFFFFFF;
        uint32_t vertexCount = stream->readUInt32();
        uint32_t unkValue = stream->readUInt32();

        size_t localStreamPos = stream->tell();
        stream->seek(dataOffset);

        std::vector<UVector3> localVertices;
        for (uint32_t j = 0; j < vertexCount; j++) {
            UVector3 vert;

            if (GetFlag(ENavMeshFlags::VertexDataCompressed)) {
                vert.x = float(stream->readUInt16()) / 65536.0f;
                vert.y = float(stream->readUInt16()) / 65536.0f;
                vert.z = float(stream->readUInt16()) / 65536.0f;

                vert = vert * mQuadtreeRoot->mExtents + mQuadtreeRoot->mBoundsMin;
            }
            else {
                vert.x = stream->readFloat();
                vert.y = stream->readFloat();
                vert.z = stream->readFloat();
            }

            localVertices.push_back(vert);
        }

        mVertices.push_back(localVertices);
        stream->seek(localStreamPos);
    }

    stream->seek(streamPos);
}

void UNavmeshData::ReadIndexData(bStream::CStream* stream, UNavAttribute* attribute) {
    size_t streamPos = stream->tell();
    stream->seek(attribute->mDescriptorsOffset);

    for (uint32_t i = 0; i < attribute->mPoolCount; i++) {
        uint64_t dataOffset = stream->readUInt64() & 0x0FFFFFFF;
        uint32_t indexCount = stream->readUInt32();
        uint32_t unkValue = stream->readUInt32();

        size_t localStreamPos = stream->tell();
        stream->seek(dataOffset);

        for (uint32_t j = 0; j < indexCount; j++) {
            mIndices.push_back(stream->readUInt16());
        }

        stream->seek(localStreamPos);
    }

    stream->seek(streamPos);
}

void UNavmeshData::ReadPolyAdjacencyData(bStream::CStream* stream, UNavAttribute* attribute) {
    size_t streamPos = stream->tell();
    stream->seek(attribute->mDescriptorsOffset);

    for (uint32_t i = 0; i < attribute->mPoolCount; i++) {
        uint64_t dataOffset = stream->readUInt64() & 0x0FFFFFFF;
        uint32_t unk2Count = stream->readUInt32();
        uint32_t unkValue = stream->readUInt32();

        size_t localStreamPos = stream->tell();
        stream->seek(dataOffset);

        for (uint32_t j = 0; j < unk2Count; j++) {
            UNavAdjacentPolyData adjData;
            uint32_t bitfield1 = stream->readUInt32();
            uint32_t bitfield2 = stream->readUInt32();

            adjData.mSectorIndex      = (bitfield1 & 0x0000000F);
            adjData.bUnk              = (bitfield1 & 0x00000010) >> 4;
            adjData.mPolygonIndex     = (bitfield1 & 0x000FFFE0) >> 5;

            adjData.mSectorIndex2  = (bitfield2 & 0x0000000F);
            adjData.bUnk2          = (bitfield2 & 0x00000010) >> 4;
            adjData.mPolygonIndex2 = (bitfield2 & 0x000FFFE0) >> 5;

            assert(!adjData.bUnk);
            assert(!adjData.bUnk2);

            mAdjPolygons.push_back(adjData);
        }

        stream->seek(localStreamPos);
    }

    stream->seek(streamPos);
}

void UNavmeshData::ReadPolygonData(bStream::CStream* stream, UNavAttribute* attribute) {
    size_t streamPos = stream->tell();
    stream->seek(attribute->mDescriptorsOffset);

    for (uint32_t i = 0; i < attribute->mPoolCount; i++) {
        uint64_t dataOffset = stream->readUInt64() & 0x0FFFFFFF;
        uint32_t polygonCount = stream->readUInt32();
        uint32_t unkValue = stream->readUInt32();

        size_t localStreamPos = stream->tell();
        stream->seek(dataOffset);
        
        for (uint32_t j = 0; j < polygonCount; j++) {
            UNavPolygon poly;

            poly.mFlags = stream->readUInt16();

            uint16_t bitfield0002 = stream->readUInt16();
            poly.mVertexCount = (bitfield0002 & 0x01F0) >> 5;

            poly.mFirstVertexIndex = stream->readUInt32();

            poly.m0008 = stream->readUInt64();
            poly.m0010 = stream->readUInt64();

            poly.mBoundsMin.x = stream->readInt16() * 0.25f;
            poly.mBoundsMax.x = stream->readInt16() * 0.25f;

            poly.mBoundsMin.y = stream->readInt16() * 0.25f;
            poly.mBoundsMax.y = stream->readInt16() * 0.25f;

            poly.mBoundsMin.z = stream->readInt16() * 0.25f;
            poly.mBoundsMax.z = stream->readInt16() * 0.25f;

            poly.m0024 = stream->readUInt16();
            poly.m0026 = stream->readUInt16();
            poly.m0028 = stream->readUInt16();
            poly.m002A = stream->readUInt16();
            poly.m002C = stream->readUInt16();

            poly.m002E = stream->readUInt8();
            poly.m002F = stream->readUInt8();

            poly.m0030 = stream->readUInt16();

            poly.m0032 = stream->readUInt16();
            poly.m0034 = stream->readUInt16();
            poly.m0036 = stream->readUInt16();

            mPolygons.push_back(poly);
        }

        stream->seek(localStreamPos);
    }

    stream->seek(streamPos);
}

void UNavmeshData::ReadUnk4Data(bStream::CStream* stream, uint64_t offset, uint32_t count) {
    size_t streamPos = stream->tell();
    stream->seek(offset);

    for (uint32_t i = 0; i < count; i++) {
        UNavUnk4 n;

        n.m0000 = stream->readUInt16();

        n.m0002.x = stream->readInt16();
        n.m0002.y = stream->readInt16();
        n.m0002.z = stream->readInt16();

        n.m0008.x = stream->readInt16();
        n.m0008.y = stream->readInt16();
        n.m0008.z = stream->readInt16();

        n.m000E = stream->readUInt16();
        n.m0010 = stream->readUInt16();
        n.m0012 = stream->readUInt16();
        n.m0014 = stream->readUInt16();

        n.m0016 = stream->readUInt8();
        n.m0017 = stream->readUInt8();
        n.m0018 = stream->readUInt8();
        n.m0019 = stream->readUInt8();

        n.m001A = stream->readUInt16();
        n.m001C = stream->readUInt32();

        mUnk4.push_back(n);
    }

    stream->seek(streamPos);
}

void UNavmeshData::ReadUnk5Data(bStream::CStream* stream, uint64_t offset, uint32_t count) {
    size_t streamPos = stream->tell();
    stream->seek(offset);

    for (int i = 0; i < count; i++) {
        mUnk5.push_back(stream->readUInt16());
    }

    stream->seek(streamPos);
}

void UNavmeshData::Debug_DumpToObj(std::string objFile) {
    std::stringstream stream;
    
    for (int i = 0; i < mVertices.size(); i++) {
        for (const UVector3& v : mVertices[i]) {
            stream << "v " << v.x << " " << v.y << " " << v.z << "\n";
        }
    }

    stream << "\n";

    for (const UNavPolygon& n : mPolygons) {
        stream << "f ";

        for (int vrtIdx = 0; vrtIdx < n.mVertexCount; vrtIdx++) {
            stream << mIndices[n.mFirstVertexIndex + vrtIdx] + 1 << " ";
        }

        stream << "\n";
    }

    bStream::CFileStream objStream(objFile, bStream::Out);
    objStream.writeString(stream.str());
}

void UNavmeshData::Debug_DumpQuadtreeToObj(std::string objFile) {
    std::stringstream stream;

    for (int i = 0; i < mVertices.size(); i++) {
        for (const UVector3& v : mVertices[i]) {
            stream << "v " << v.x << " " << v.y << " " << v.z << "\n";
        }
    }

    stream << "\n";

    mQuadtreeRoot->Debug_DumpQuadtreeNodeToObj(stream, mPolygons, mIndices);

    bStream::CFileStream objStream(objFile, bStream::Out);
    objStream.writeString(stream.str());
}

void UNavmeshData::Deserialize(bStream::CStream* stream) {
    if (stream == nullptr) {
        return;
    }

    size_t streamPos = 0;
    mVTablePtr = stream->readUInt64();

    // Read blockmap
    uint64_t blockMapPtr = stream->readUInt64() & 0x0FFFFFFF;
    if (blockMapPtr != 0) {
        streamPos = stream->tell();
        stream->seek(blockMapPtr);
        //mBlockMap.Deserialize(stream);

        stream->seek(streamPos);
    }

    mFlags = stream->readUInt32();
    mFileVersion = stream->readUInt32();

    m0018 = stream->readUInt64();

    mTransformMatrix.r0.x = stream->readFloat();
    mTransformMatrix.r0.y = stream->readFloat();
    mTransformMatrix.r0.z = stream->readFloat();
    mTransformMatrix.r0.w = stream->readFloat();

    mTransformMatrix.r1.x = stream->readFloat();
    mTransformMatrix.r1.y = stream->readFloat();
    mTransformMatrix.r1.z = stream->readFloat();
    mTransformMatrix.r1.w = stream->readFloat();

    mTransformMatrix.r2.x = stream->readFloat();
    mTransformMatrix.r2.y = stream->readFloat();
    mTransformMatrix.r2.z = stream->readFloat();
    mTransformMatrix.r2.w = stream->readFloat();

    m0050 = stream->readUInt64();
    m0058 = stream->readUInt64();

    mMeshExtents.x = stream->readFloat();
    mMeshExtents.y = stream->readFloat();
    mMeshExtents.z = stream->readFloat();
    mMeshExtents.w = stream->readFloat();

    uint64_t vtxAttributeOffset = stream->readUInt64() & 0x0FFFFFFF;
    streamPos = stream->tell();
    stream->seek(vtxAttributeOffset);
    UNavAttribute vtxAttribute;
    vtxAttribute.Deserialize(stream);
    stream->seek(streamPos);

    m0078 = stream->readUInt64();

    uint64_t indexAttributeOffset = stream->readUInt64() & 0x0FFFFFFF;
    streamPos = stream->tell();
    stream->seek(indexAttributeOffset);
    UNavAttribute indexAttribute;
    indexAttribute.Deserialize(stream);
    stream->seek(streamPos);

    uint64_t unk2AttributeOffset = stream->readUInt64() & 0x0FFFFFFF;
    streamPos = stream->tell();
    stream->seek(unk2AttributeOffset);
    UNavAttribute unk2Attribute;
    unk2Attribute.Deserialize(stream);
    stream->seek(streamPos);

    mIndexCount = stream->readUInt32();

    uint32_t adjacentMeshCount = stream->readUInt32();
    for (int i = 0; i < 32; i++) {
        uint32_t adjacentMeshId = stream->readUInt32();

        if (adjacentMeshId != 0) {
            mAdjacentMeshIds.push_back(adjacentMeshId);
        }
    }

    uint64_t polygonAttributeOffset = stream->readUInt64() & 0x0FFFFFFF;
    streamPos = stream->tell();
    stream->seek(polygonAttributeOffset);
    UNavAttribute polygonAttribute;
    polygonAttribute.Deserialize(stream);
    stream->seek(streamPos);

    uint64_t quadtreeOffset = stream->readUInt64() & 0x0FFFFFFF;
    streamPos = stream->tell();
    stream->seek(quadtreeOffset);

    mQuadtreeRoot = std::make_shared<UNavQuadtreeNode>();
    mQuadtreeRoot->Deserialize(stream);

    stream->seek(streamPos);

    uint64_t unk4Offset = stream->readUInt64() & 0x0FFFFFFF;
    uint64_t unk5Offset = stream->readUInt64() & 0x0FFFFFFF;

    mVertexCount = stream->readUInt32();
    mPolygonCount = stream->readUInt32();

    mSectorIndex = stream->readUInt32();
    mSectorY = (mSectorIndex / 215);
    mSectorX = mSectorIndex - (mSectorY * 215);
    mSectorY *= 3;
    mSectorX *= 3;

    m0144 = stream->readUInt32();
    m0148 = stream->readUInt32();

    mUnk4Count = stream->readUInt32();
    mUnk5Count = stream->readUInt32();

    ReadVertexData(stream, &vtxAttribute);
    ReadIndexData(stream, &indexAttribute);
    ReadPolyAdjacencyData(stream, &unk2Attribute);
    ReadPolygonData(stream, &polygonAttribute);

    if (GetFlag(ENavMeshFlags::HasSpecialLinks)) {
        ReadUnk4Data(stream, unk4Offset, mUnk4Count);
        ReadUnk5Data(stream, unk5Offset, mUnk5Count);
    }
}

void UNavmeshData::Serialize(bStream::CMemoryStream* stream) {

}

void UNavQuadtreeLeafData::Deserialize(bStream::CStream* stream) {
    mRuntimePtr = stream->readUInt64() & 0x0FFFFFFF;

    uint64_t polygonIndicesOffset = stream->readUInt64() & 0x0FFFFFFF;
    uint64_t boundsOffset = stream->readUInt64() & 0x0FFFFFFF;

    uint16_t polygonCount = stream->readUInt16();
    uint16_t boundsCount = stream->readUInt16();

    stream->seek(polygonIndicesOffset);
    for (int i = 0; i < polygonCount; i++) {
        mPolygonIndices.push_back(stream->readUInt16());
    }

    if (boundsCount != 0) {
        assert(false);
    }
}

void UNavQuadtreeNode::Deserialize(bStream::CStream* stream) {
    mBoundsMin.x = stream->readFloat();
    mBoundsMin.y = stream->readFloat();
    mBoundsMin.z = stream->readFloat();
    stream->skip(4);

    mBoundsMax.x = stream->readFloat();
    mBoundsMax.y = stream->readFloat();
    mBoundsMax.z = stream->readFloat();
    stream->skip(4);

    mExtents = mBoundsMax - mBoundsMin;

    mAABBMin.x = stream->readInt16() * 0.25f;
    mAABBMax.x = stream->readInt16() * 0.25f;
    mAABBMin.y = stream->readInt16() * 0.25f;
    mAABBMax.y = stream->readInt16() * 0.25f;
    mAABBMin.z = stream->readInt16() * 0.25f;
    mAABBMax.z = stream->readInt16() * 0.25f;

    uint64_t leafDataOffset = stream->readUInt64() & 0x0FFFFFFF;
    size_t streamPos = 0;

    if (leafDataOffset != 0) {
        mLeafData = std::make_shared<UNavQuadtreeLeafData>();

        stream->seek(leafDataOffset);
        mLeafData->Deserialize(stream);

        return;
    }

    for (uint32_t i = 0; i < 4; i++) {
        uint64_t childNodeOffset = stream->readUInt64() & 0x0FFFFFFF;

        if (childNodeOffset == 0) {
            continue;
        }

        streamPos = stream->tell();
        stream->seek(childNodeOffset);

        mChildren[i] = std::make_shared<UNavQuadtreeNode>();
        mChildren[i]->Deserialize(stream);

        stream->seek(streamPos);
    }
}

void UNavQuadtreeNode::Debug_DumpQuadtreeNodeToObj(std::stringstream& stream, const std::vector<UNavPolygon>& polygons, const std::vector<uint16_t>& vertexIndices) {
    if (mLeafData == nullptr) {
        for (uint32_t i = 0; i < 4; i++) {
            if (mChildren[i] == nullptr) {
                continue;
            }

            mChildren[i]->Debug_DumpQuadtreeNodeToObj(stream, polygons, vertexIndices);
        }

        return;
    }

    stream << "o node_" << mAABBMax.x << "\n";

    for (uint32_t i = 0; i < mLeafData->mPolygonIndices.size(); i++) {
        const UNavPolygon& poly = polygons[mLeafData->mPolygonIndices[i]];

        stream << "f ";
        for (uint32_t j = 0; j < poly.mVertexCount; j++) {
            stream << vertexIndices[poly.mFirstVertexIndex + j] + 1 << " ";
        }
        stream << "\n";
    }
}
