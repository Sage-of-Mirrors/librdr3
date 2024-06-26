#include "navmesh/navmeshdata.hpp"
#include "navmesh/quadtree.hpp"
#include "util/streamutil.hpp"
#include "util/bstream.h"

/* === Data deserialization functions === */

void DeserializeVertex(bStream::CStream* stream, std::shared_ptr<Vector3> vector) {
    vector->x = float(stream->readUInt16()) / 65536.0f;
    vector->y = float(stream->readUInt16()) / 65536.0f;
    vector->z = float(stream->readUInt16()) / 65536.0f;
}

void DeserializeVertexIndex(bStream::CStream* stream, std::shared_ptr<uint16_t> index) {
    *index = stream->readUInt16();
}

void DeserializeAdjPolygonData(bStream::CStream* stream, std::shared_ptr<CNavAdjacentPolyData> data) {
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

void DeserializePolygon(bStream::CStream* stream, std::shared_ptr<CNavPolygon> poly) {
    uint32_t flags1 = stream->readUInt32();
    poly->mInfo1.mFlags       = (flags1 & 0x001FFFFF);
    poly->mInfo1.mVertexCount = (flags1 & 0x01E00000) >> 21;
    poly->mInfo1.mPointType   = (flags1 & 0xFE000000) >> 25;

    uint32_t flags2 = stream->readUInt32();
    poly->mInfo2.mFirstVertexIndex = (flags2 & 0x0000FFFF);
    poly->mInfo2.mNavMeshIndex     = (flags2 & 0x7FFF0000) >> 16;

    // Runtime fields that we don't care about
    assert(stream->readUInt64() == 0);
    assert(stream->readUInt64() == 0);

    poly->mBoundsMin.x = stream->readInt16() * 0.25f;
    poly->mBoundsMax.x = stream->readInt16() * 0.25f;
    poly->mBoundsMin.y = stream->readInt16() * 0.25f;
    poly->mBoundsMax.y = stream->readInt16() * 0.25f;
    poly->mBoundsMin.z = stream->readInt16() * 0.25f;
    poly->mBoundsMax.z = stream->readInt16() * 0.25f;

    uint32_t flags3 = stream->readUInt32();
    poly->mInfo3.mAudioProperties       = (flags3 & 0x00000008);
    poly->mInfo3.bDebug                 = (flags3 & 0x00000010) >> 4;
    poly->mInfo3.bNearVehicle           = (flags3 & 0x00000020) >> 5;
    poly->mInfo3.bInterior              = (flags3 & 0x00000040) >> 6;
    poly->mInfo3.bIsolated              = (flags3 & 0x00000080) >> 7;
    poly->mInfo3.bZeroStitchDLC         = (flags3 & 0x00000100) >> 8;
    poly->mInfo3.bNetworkSpawnCandidate = (flags3 & 0x00000200) >> 9;
    poly->mInfo3.bIsRoad                = (flags3 & 0x00000400) >> 10;
    poly->mInfo3.bOnEdgeOfMesh          = (flags3 & 0x00000800) >> 11;
    poly->mInfo3.bIsTrainTrack          = (flags3 & 0x00001000) >> 12;
    poly->mInfo3.bIsShallowWater        = (flags3 & 0x00002000) >> 13;
    poly->mInfo3.mPedDensity            = (flags3 & 0x0001C000) >> 15;

    poly->mCentroidX = float(stream->readUInt8()) / 256.0f;
    poly->mCentroidY = float(stream->readUInt8()) / 256.0f;

    poly->m002A = stream->readUInt8();
    poly->m002B = stream->readUInt8();
    poly->m002C = stream->readUInt8();
    poly->m002D = stream->readUInt8();
    poly->m002E = stream->readUInt8();
    poly->m002F = stream->readUInt8();

    poly->m0030 = stream->readUInt16();

    poly->m0032 = stream->readUInt16();
    poly->m0034 = stream->readUInt16();
    poly->m0036 = stream->readUInt16();
}

void DeserializeLink(bStream::CStream* stream, std::shared_ptr<CNavLink> link) {
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

void SerializeVertex(bStream::CStream* stream, std::shared_ptr<Vector3> vector) {
    stream->writeUInt16(uint16_t(vector->x * 65536.0f));
    stream->writeUInt16(uint16_t(vector->y * 65536.0f));
    stream->writeUInt16(uint16_t(vector->z * 65536.0f));
}

void SerializeVertexIndex(bStream::CStream* stream, std::shared_ptr<uint16_t> index) {
    stream->writeUInt16(*index);
}

void SerializeAdjPolygonData(bStream::CStream* stream, std::shared_ptr<CNavAdjacentPolyData> data) {
    uint32_t bitfield1 = 0;
    uint32_t bitfield2 = 0;

    bitfield1 |= (data->mSectorIndex & 0x0000000F);
    bitfield1 |= (data->bUnk & 0x00000001) << 4;
    bitfield1 |= (data->mPolygonIndex & 0x00007FFF) << 5;
    bitfield1 |= (data->mAdjacencyType & 0x00000003) << 20;
    bitfield1 |= (int16_t(data->mSpaceAroundVertex / 0.25f) & 0x0000001F) << 22;
    bitfield1 |= (int16_t(data->mSpaceBeyondEdge   / 0.25f) & 0x0000001F) << 27;

    stream->writeUInt32(bitfield1);
    stream->writeUInt32(bitfield2);
}

void SerializePolygon(bStream::CStream* stream, std::shared_ptr<CNavPolygon> poly) {
    // TODO: Serialize polygons
    for (int i = 0; i < 0x38; i++) {
        stream->writeUInt8(0);
    }
}

void SerializeLink(bStream::CStream* stream, std::shared_ptr<CNavLink> link) {
    // TODO: Serialize links
    for (int i = 0; i < 0x20; i++) {
        stream->writeUInt8(0);
    }
}

/* === CNavmeshData === */

CNavmeshData::CNavmeshData() {

}

CNavmeshData::~CNavmeshData() {

}

void CNavmeshData::Deserialize(bStream::CStream* stream) {
    if (stream == nullptr) {
        return;
    }

    assert(stream->readUInt64() == 0); // VTable pointer, always 0

    // Read blockmap
    uint64_t blockMapPtr = StreamUtil::DeserializePtr64(stream);
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
    StreamUtil::DeserializeMatrix3x4(stream, mTransformMatrix);
    assert(stream->readUInt64() == 0); // Padding
    assert(stream->readUInt64() == 0); // Padding

    // Mesh extents
    StreamUtil::DeserializeVector3(stream, mMeshExtents);
    stream->skip(4);

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
    mQuadtreeRoot = std::make_shared<CNavQuadtreeNode>();
    mQuadtreeRoot->Deserialize(stream);

    uint64_t linksOffset = StreamUtil::DeserializePtr64(stream);
    uint64_t linkIndicesOffset = StreamUtil::DeserializePtr64(stream);

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
    if (GetFlag(ENavMeshFlags::HAS_LINKS)) {
        stream->seek(linksOffset);
        for (uint32_t i = 0; i < mLinkCount; i++) {
            std::shared_ptr<CNavLink> link = std::make_shared<CNavLink>();
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

void CNavmeshData::ProcessData() {
    mSectorY = (mSectorIndex / 215);
    mSectorX = mSectorIndex - (mSectorY * 215);
    mSectorY *= 3;
    mSectorX *= 3;

    for (uint32_t i = 0; i < mVertices.GetTotalSize(); i++) {
        Vector3* vertex = mVertices[i].get();
        *vertex = *vertex * (mQuadtreeRoot->mBoundsMax - mQuadtreeRoot->mBoundsMin) + mQuadtreeRoot->mBoundsMin;
    }
}

void CNavmeshData::Serialize(bStream::CStream* stream) {
    stream->writeUInt64(0); // VTable pointer
    stream->writeUInt64(0); // Block map pointer, TODO
    stream->writeUInt32(mFlags);
    stream->writeUInt32(RDR3_NAVMESH_VERSION);
    stream->writeUInt64(0); // Padding

    StreamUtil::SerializeMatrix3x4(stream, mTransformMatrix);
    StreamUtil::PadStream(stream, 32);

    StreamUtil::SerializeVector3(stream, mMeshExtents);
    StreamUtil::PadStream(stream, 16);

    stream->writeUInt64(0); // Placeholder for vertex array pointer
    stream->writeUInt64(0); // Placeholder for unused uncompressed vertex array pointer
    stream->writeUInt64(0); // Placeholder for vertex index array pointer
    stream->writeUInt64(0); // Placeholder for adjacency array pointer

    stream->writeUInt32(mVertexIndices.GetTotalSize());
    stream->writeUInt32(0); // Placeholder for adjacent mesh count

    uint32_t adjacentMeshCount = 0;
    for (uint32_t i = 0; i < ADJACENT_NAVMESHES_MAX; i++) {
        stream->writeUInt32(mAdjacentMeshIds[i]);

        if (mAdjacentMeshIds[i] != 0) {
            adjacentMeshCount++;
        }
    }

    size_t curPos = stream->tell();

    stream->seek(0x94);
    stream->writeUInt32(adjacentMeshCount);
    stream->seek(curPos);

    stream->writeUInt64(0); // Placeholder for polygon array pointer
    stream->writeUInt64(0); // Placeholder for quadtree pointer

    stream->writeUInt64(0); // Placeholder for links pointer
    stream->writeUInt64(0); // Placeholder for links index pointer

    stream->writeUInt32(mVertices.GetTotalSize());
    stream->writeUInt32(mPolygons.GetTotalSize());
    stream->writeUInt32(mSectorIndex);

    stream->writeUInt32(0); // Placeholder for total data size
    stream->writeUInt32(0); // Unused size value

    stream->writeUInt32(uint32_t(mLinks.size()));
    stream->writeUInt32(uint32_t(mLinkIndices.size()));

    stream->writeUInt32(0); // Padding
    stream->writeUInt32(0); // Padding
    stream->writeUInt32(0); // Padding

    stream->writeUInt32(0); // Placeholder for build id... can we calculate this?

    StreamUtil::PadStream(stream, 16);
    uint32_t totalDataWritten = 0;
    
    if (mLinks.size() > 0) {
        // Write links
        curPos = stream->tell();

        stream->seek(0x128);
        StreamUtil::SerializePtr64(stream, uint64_t(curPos));
        stream->seek(curPos);

        for (size_t i = 0; i < mLinks.size(); i++) {
            SerializeLink(stream, mLinks[i]);
        }

        StreamUtil::PadStream(stream, 16);
        totalDataWritten += uint32_t(stream->tell() - curPos);

        // Write link indices
        curPos = stream->tell();

        stream->seek(0x0130);
        StreamUtil::SerializePtr64(stream, uint64_t(curPos));
        stream->seek(curPos);

        for (size_t i = 0; i < mLinkIndices.size(); i++) {
            stream->writeUInt16(mLinkIndices[i]);
        }

        StreamUtil::PadStream(stream, 16);
        totalDataWritten += uint32_t(stream->tell() - curPos);
    }

    totalDataWritten += mAdjPolygons.SerializeData(stream, SerializeAdjPolygonData);
    totalDataWritten += mPolygons.SerializeData(stream, SerializePolygon);

    // TODO: serialize quadtree here and update totalDataWritten

    totalDataWritten += mVertexIndices.SerializeData(stream, SerializeVertexIndex);
    totalDataWritten += mVertices.SerializeData(stream, SerializeVertex);

    uint64_t adjPolyOffset = mAdjPolygons.SerializeMetadata(stream,      0x40828888);
    uint64_t polyArrayOffset = mPolygons.SerializeMetadata(stream,       0x40828898);
    uint64_t indexArrayOffset = mVertexIndices.SerializeMetadata(stream, 0x40828868);
    uint64_t vertexArrayOffset = mVertices.SerializeMetadata(stream,     0x40828878);

    StreamUtil::PadStream(stream, 16);

    stream->seek(0x0070);
    StreamUtil::SerializePtr64(stream, vertexArrayOffset);
    
    stream->seek(0x0080);
    StreamUtil::SerializePtr64(stream, indexArrayOffset);

    stream->seek(0x0088);
    StreamUtil::SerializePtr64(stream, adjPolyOffset);

    stream->seek(0x0118);
    StreamUtil::SerializePtr64(stream, polyArrayOffset);

    stream->seek(0x0144);
    stream->writeUInt32(totalDataWritten);
}

void CNavmeshData::GetVertices(float*& vtxData, uint32_t*& indexData, uint32_t& vtxCount, uint32_t& indexCount) {
    uint32_t vtxSize = (sizeof(Vector3) + sizeof(Vector3));
    std::vector<Vector3> vtxs;
    std::vector<uint32_t> idxs;

    for (uint32_t polyIdx = 0; polyIdx < mPolygons.GetTotalSize(); polyIdx++) {
        std::shared_ptr<CNavPolygon> poly = mPolygons[polyIdx];
        if (poly->mInfo1.mVertexCount < 3) {
            continue;
        }

        switch (poly->mInfo1.mVertexCount) {
            case 1:
            case 2:
            {
                break; // Discard degenerate polys for rendering
            }
            case 3:
            {
                Vector3 a = (*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 0]]).toZUp();
                Vector3 b = (*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 1]]).toZUp();
                Vector3 c = (*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 2]]).toZUp();

                Vector3 ab = a - b;
                Vector3 bc = b - c;

                Vector3 polyNormal = ab.cross(bc).normalized();
                if (polyNormal.y < 0) {
                    polyNormal = bc.cross(ab).normalized();
                }

                uint32_t baseIdx = uint32_t(vtxs.size() / 2);
                idxs.push_back(baseIdx);
                idxs.push_back(baseIdx + 1);
                idxs.push_back(baseIdx + 2);

                vtxs.push_back((*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 0]]).toZUp());
                vtxs.push_back(polyNormal);
                vtxs.push_back((*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 1]]).toZUp());
                vtxs.push_back(polyNormal);
                vtxs.push_back((*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 2]]).toZUp());
                vtxs.push_back(polyNormal);

                break;
            }
            default:
            {
                for (uint32_t vtxIdx = 0; vtxIdx < poly->mInfo1.mVertexCount - 2; vtxIdx++) {
                    Vector3 a = (*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 0]]).toZUp();
                    Vector3 b = (*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + vtxIdx + 1]]).toZUp();
                    Vector3 c = (*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + vtxIdx + 2]]).toZUp();

                    Vector3 ab = a - b;
                    Vector3 bc = b - c;

                    Vector3 polyNormal = ab.cross(bc).normalized();

                    uint32_t baseIdx = uint32_t(vtxs.size() / 2);
                    idxs.push_back(baseIdx);
                    idxs.push_back(baseIdx + 1);
                    idxs.push_back(baseIdx + 2);

                    vtxs.push_back((*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 0]]).toZUp());
                    vtxs.push_back(polyNormal);
                    vtxs.push_back((*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + vtxIdx + 1]]).toZUp());
                    vtxs.push_back(polyNormal);
                    vtxs.push_back((*mVertices[*mVertexIndices[poly->mInfo2.mFirstVertexIndex + vtxIdx + 2]]).toZUp());
                    vtxs.push_back(polyNormal);
                }

                break;
            }
        }
    }

    vtxCount = uint32_t(vtxs.size() / 2);
    vtxData = new float[vtxCount * vtxSize];
    std::memcpy(vtxData, vtxs.data(), vtxCount * vtxSize);

    indexCount = uint32_t(idxs.size());
    indexData = new uint32_t[indexCount];
    std::memcpy(indexData, idxs.data(), indexCount * sizeof(uint32_t));
}

void CNavmeshData::GetIndicesForPolys(uint32_t*& data, uint32_t& indexCount) {
    std::vector<uint32_t> triIndices;

    for (uint32_t polyIdx = 0; polyIdx < mPolygons.GetTotalSize(); polyIdx++) {
        std::shared_ptr<CNavPolygon> poly = mPolygons[polyIdx];

        switch (poly->mInfo1.mVertexCount) {
            case 1:
            case 2:
            {
                break; // Discard degenerate polys for rendering
            }
            case 3:
            {
                indexCount += 3;

                triIndices.push_back(*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 0]);
                triIndices.push_back(*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 1]);
                triIndices.push_back(*mVertexIndices[poly->mInfo2.mFirstVertexIndex + 2]);

                break;
            }
            default:
            {
                indexCount += (poly->mInfo1.mVertexCount - 2) * 3;

                for (uint32_t vtxIdx = 0; vtxIdx < poly->mInfo1.mVertexCount - 2; vtxIdx++) {
                    triIndices.push_back(*mVertexIndices[poly->mInfo2.mFirstVertexIndex]);
                    triIndices.push_back(*mVertexIndices[poly->mInfo2.mFirstVertexIndex + vtxIdx + 1]);
                    triIndices.push_back(*mVertexIndices[poly->mInfo2.mFirstVertexIndex + vtxIdx + 2]);
                }

                break;
            }
        }
    }

    data = new uint32_t[triIndices.size() * sizeof(uint32_t)];
    std::memcpy(data, triIndices.data(), triIndices.size() * sizeof(uint32_t));
}

void CNavmeshData::Debug_DumpToObj(std::string objFile) {
    std::stringstream stream;

    for (uint32_t i = 0; i < mVertices.GetTotalSize(); i++) {
        stream << "v " << mVertices[i]->x << " " << mVertices[i]->y << " " << mVertices[i]->z << "\n";
    }

    stream << "\n";

    for (uint32_t i = 0; i < mPolygons.GetTotalSize(); i++) {
        stream << "f ";

        for (int vrtIdx = 0; vrtIdx < mPolygons[i]->mInfo1.mVertexCount; vrtIdx++) {
            stream << *mVertexIndices[mPolygons[i]->mInfo2.mFirstVertexIndex + vrtIdx] + 1 << " ";
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
