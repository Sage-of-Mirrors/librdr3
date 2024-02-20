#include "ynv/navmeshdata.hpp"
#include "ynv/quadtree.hpp"
#include "util/streamutil.hpp"

#include <bstream.h>

/* === Data deserialization functions === */

void UNavmesh::DeserializeVertex(bStream::CStream* stream, std::shared_ptr<UVector3> vector) {
    vector->x = float(stream->readUInt16()) / 65536.0f;
    vector->y = float(stream->readUInt16()) / 65536.0f;
    vector->z = float(stream->readUInt16()) / 65536.0f;
}

void UNavmesh::DeserializeVertexIndex(bStream::CStream* stream, std::shared_ptr<uint16_t> index) {
    *index = stream->readUInt16();
}

void UNavmesh::DeserializeAdjPolygonData(bStream::CStream* stream, std::shared_ptr<UNavAdjacentPolyData> data) {
    uint32_t bitfield1 = stream->readUInt32();
    uint32_t bitfield2 = stream->readUInt32();

    data->mSectorIndex       = (bitfield1 & 0x0000000F);
    data->bUnk               = (bitfield1 & 0x00000010)  >> 4;
    data->mPolygonIndex      = (bitfield1 & 0x000FFFE0)  >> 5;
    data->mAdjacencyType     = (bitfield1 & 0x00300000)  >> 20;
    data->mSpaceAroundVertex = ((bitfield1 & 0x07C00000) >> 22) * 0.25f;
    data->mSpaceBeyondEdge   = ((bitfield1 & 0xF8000000) >> 27) * 0.25f;

    data->mSectorIndex2      = (bitfield2 & 0x0000000F);
    data->bUnk2              = (bitfield2 & 0x00000010)  >> 4;
    data->mPolygonIndex2     = (bitfield2 & 0x000FFFE0)  >> 5;
    data->bAdjacencyDisabled = (bitfield2 & 0x00100000)  >> 20;
    data->bProvidesCover     = (bitfield2 & 0x00200000)  >> 21;
    data->bHighDrop          = (bitfield2 & 0x00400000)  >> 22;
    data->bExternal          = (bitfield2 & 0x00800000)  >> 23;
}

void UNavmesh::DeserializePolygon(bStream::CStream* stream, std::shared_ptr<UNavPolygon> poly) {
    poly->mFlags = stream->readUInt16();

    uint16_t bitfield0002 = stream->readUInt16();
    poly->mVertexCount = (bitfield0002 & 0x01F0) >> 5;
    poly->mFirstVertexIndex = stream->readUInt32();

    poly->m0008 = stream->readUInt64();
    poly->m0010 = stream->readUInt64();

    poly->mBoundsMin.x = stream->readInt16() * 0.25f;
    poly->mBoundsMax.x = stream->readInt16() * 0.25f;

    poly->mBoundsMin.y = stream->readInt16() * 0.25f;
    poly->mBoundsMax.y = stream->readInt16() * 0.25f;

    poly->mBoundsMin.z = stream->readInt16() * 0.25f;
    poly->mBoundsMax.z = stream->readInt16() * 0.25f;

    poly->m0024 = stream->readUInt16();
    poly->m0026 = stream->readUInt16();
    poly->m0028 = stream->readUInt16();
    poly->m002A = stream->readUInt16();
    poly->m002C = stream->readUInt16();

    poly->m002E = stream->readUInt8();
    poly->m002F = stream->readUInt8();

    poly->m0030 = stream->readUInt16();

    poly->m0032 = stream->readUInt16();
    poly->m0034 = stream->readUInt16();
    poly->m0036 = stream->readUInt16();
}

void UNavmesh::DeserializeLink(bStream::CStream* stream, std::shared_ptr<UNavLink> link) {
    link->m0000 = stream->readUInt16();

    link->m0002.x = stream->readInt16();
    link->m0002.y = stream->readInt16();
    link->m0002.z = stream->readInt16();

    link->m0008.x = stream->readInt16();
    link->m0008.y = stream->readInt16();
    link->m0008.z = stream->readInt16();

    link->m000E = stream->readUInt16();
    link->m0010 = stream->readUInt16();
    link->m0012 = stream->readUInt16();
    link->m0014 = stream->readUInt16();

    link->m0016 = stream->readUInt8();
    link->m0017 = stream->readUInt8();
    link->m0018 = stream->readUInt8();
    link->m0019 = stream->readUInt8();

    link->m001A = stream->readUInt16();
    link->m001C = stream->readUInt32();
}

/* === UNavmeshData === */

UNavmesh::UNavmeshData::UNavmeshData() : mQuadtreeRoot(std::make_shared<UNavQuadtreeNode>()) {

}

UNavmesh::UNavmeshData::~UNavmeshData() {

}

void UNavmesh::UNavmeshData::Deserialize(bStream::CStream* stream) {
    if (stream == nullptr) {
        return;
    }

    assert(stream->readUInt64() == 0); // VTable pointer, always 0

    // Read blockmap
    uint64_t blockMapPtr = UStreamUtil::DeserializePtr64(stream);
    if (blockMapPtr != 0) {
        size_t streamPos = stream->tell();
        stream->seek(blockMapPtr);
        //mBlockMap.Deserialize(stream);

        stream->seek(streamPos);
    }

    mFlags = stream->readUInt32();
    mFileVersion = stream->readUInt32(); // RDR3 is 0x00010013
    assert(mFileVersion == RDR3_NAVMESH_VERSION);
    assert(stream->readUInt64() == 0);

    // Transform matrix
    UStreamUtil::DeserializeMatrix3x4(stream, mTransformMatrix);
    assert(stream->readUInt64() == 0); // Padding
    assert(stream->readUInt64() == 0); // Padding

    // Mesh extents
    UStreamUtil::DeserializeVector4(stream, mMeshExtents);

    // Vertices
    mVertices.Deserialize(stream, DeserializeVertex);
    assert(stream->readUInt64() == 0); // Offset slot for uncompressed vertices, but RDR3 only has compressed vertices

    // Vertex indices
    mVertexIndices.Deserialize(stream, DeserializeVertexIndex);
    // Adjacent polygon data
    mAdjPolygons.Deserialize(stream, DeserializeAdjPolygonData);

    mIndexCount = stream->readUInt32();

    uint32_t adjacentMeshCount = stream->readUInt32();
    for (int i = 0; i < ADJACENT_NAVMESHES_MAX; i++) {
        uint32_t adjacentMeshId = stream->readUInt32();

        if (adjacentMeshId != 0) {
            mAdjacentMeshIds[i] = adjacentMeshId;
        }
    }

    // Polygons
    mPolygons.Deserialize(stream, DeserializePolygon);
    // Quadtree
    mQuadtreeRoot->Deserialize(stream);

    uint64_t linksOffset = UStreamUtil::DeserializePtr64(stream);
    uint64_t linkIndicesOffset = UStreamUtil::DeserializePtr64(stream);

    mVertexCount = stream->readUInt32();
    mPolygonCount = stream->readUInt32();

    mSectorIndex = stream->readUInt32();

    mDataSize = stream->readUInt32();
    assert(stream->readUInt32() == 0); // "numChildBounds", but RDR3 only ever has 0 here

    mLinkCount = stream->readUInt32();
    mLinkIndexCount = stream->readUInt32();

    // Padding
    assert(stream->readUInt32() == 0);
    assert(stream->readUInt32() == 0);
    assert(stream->readUInt32() == 0);

    mBuildId = stream->readUInt32();

    // Padding
    assert(stream->readUInt32() == 0);
    assert(stream->readUInt32() == 0);
    assert(stream->readUInt32() == 0);

    // Links, if the mesh has them
    if (GetFlag(ENavMeshFlags::HasLinks)) {
        stream->seek(linksOffset);
        for (uint32_t i = 0; i < mLinkCount; i++) {
            std::shared_ptr<UNavLink> link = std::make_shared<UNavLink>();
            DeserializeLink(stream, link);

            mLinks.push_back(link);
        }

        stream->seek(linkIndicesOffset);
        for (uint32_t i = 0; i < mLinkIndexCount; i++) {
            mLinkIndices.push_back(stream->readUInt16());
        }
    }

    ProcessData();
}

void UNavmesh::UNavmeshData::ProcessData() {
    mSectorY = (mSectorIndex / 215);
    mSectorX = mSectorIndex - (mSectorY * 215);
    mSectorY *= 3;
    mSectorX *= 3;

    for (uint32_t i = 0; i < mVertices.GetTotalSize(); i++) {
        UVector3* vertex = mVertices[i].get();
        *vertex = *vertex * (mQuadtreeRoot->mBoundsMax - mQuadtreeRoot->mBoundsMin) + mQuadtreeRoot->mBoundsMin;
    }
}

void UNavmesh::UNavmeshData::Serialize(bStream::CMemoryStream* stream) {

}

void UNavmesh::UNavmeshData::Debug_DumpToObj(std::string objFile) {
    std::stringstream stream;

    for (uint32_t i = 0; i < mVertices.GetTotalSize(); i++) {
        stream << "v " << mVertices[i]->x << " " << mVertices[i]->y << " " << mVertices[i]->z << "\n";
    }

    stream << "\n";

    for (uint32_t i = 0; i < mPolygons.GetTotalSize(); i++) {
        stream << "f ";

        for (int vrtIdx = 0; vrtIdx < mPolygons[i]->mVertexCount; vrtIdx++) {
            stream << *mVertexIndices[mPolygons[i]->mFirstVertexIndex + vrtIdx] + 1 << " ";
        }

        stream << "\n";
    }

    bStream::CFileStream objStream(objFile, bStream::Out);
    objStream.writeString(stream.str());
}

//void UNavmesh::UNavmeshData::Debug_DumpQuadtreeToObj(std::string objFile) {
//    std::stringstream stream;
//
//    for (uint32_t i = 0; i < mVertices.GetTotalSize(); i++) {
//        stream << "v " << mVertices[i]->x << " " << mVertices[i]->y << " " << mVertices[i]->z << "\n";
//    }
//
//    stream << "\n";
//
//    //mQuadtreeRoot->Debug_DumpQuadtreeNodeToObj(stream, mPolygons, mIndices);
//
//    bStream::CFileStream objStream(objFile, bStream::Out);
//    objStream.writeString(stream.str());
//}
