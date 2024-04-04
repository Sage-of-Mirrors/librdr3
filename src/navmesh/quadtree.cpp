#include "navmesh/quadtree.hpp"
#include "navmesh/navmeshdata.hpp"
#include "util/streamutil.hpp"
#include "util/bstream.h"

void CNavQuadtreeLeafData::Deserialize(bStream::CStream* stream) {
    mRuntimePtr = StreamUtil::DeserializePtr64(stream);

    uint64_t polygonIndicesOffset = StreamUtil::DeserializePtr64(stream);
    uint64_t boundsOffset = StreamUtil::DeserializePtr64(stream);

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

void CNavQuadtreeNode::Deserialize(bStream::CStream* stream) {
    uint64_t quadtreeOffset = StreamUtil::DeserializePtr64(stream);

    size_t returnPos = stream->tell();
    stream->seek(quadtreeOffset);

    Deserialize_Recursive(stream);

    stream->seek(returnPos);
}

void CNavQuadtreeNode::Deserialize_Recursive(bStream::CStream* stream) {
    StreamUtil::DeserializeVector3(stream, mBoundsMin);
    stream->skip(4);

    StreamUtil::DeserializeVector3(stream, mBoundsMax);
    stream->skip(4);

    mExtents = mBoundsMax - mBoundsMin;

    mAABBMin.x = stream->readInt16() * 0.25f;
    mAABBMax.x = stream->readInt16() * 0.25f;
    mAABBMin.y = stream->readInt16() * 0.25f;
    mAABBMax.y = stream->readInt16() * 0.25f;
    mAABBMin.z = stream->readInt16() * 0.25f;
    mAABBMax.z = stream->readInt16() * 0.25f;

    uint64_t leafDataOffset = StreamUtil::DeserializePtr64(stream);
    size_t streamPos = 0;

    if (leafDataOffset != 0) {
        mLeafData = std::make_shared<CNavQuadtreeLeafData>();

        stream->seek(leafDataOffset);
        mLeafData->Deserialize(stream);

        return;
    }

    for (uint32_t i = 0; i < 4; i++) {
        uint64_t childNodeOffset = StreamUtil::DeserializePtr64(stream);

        if (childNodeOffset == 0) {
            continue;
        }

        streamPos = stream->tell();
        stream->seek(childNodeOffset);

        mChildren[i] = std::make_shared<CNavQuadtreeNode>();
        mChildren[i]->Deserialize_Recursive(stream);

        stream->seek(streamPos);
    }
}

//void UNavmesh::UNavQuadtreeNode::Debug_DumpQuadtreeNodeToObj(std::stringstream& stream, const std::vector<UNavPolygon>& polygons, const std::vector<uint16_t>& vertexIndices) {
//    if (mLeafData == nullptr) {
//        for (uint32_t i = 0; i < 4; i++) {
//            if (mChildren[i] == nullptr) {
//                continue;
//            }
//
//            mChildren[i]->Debug_DumpQuadtreeNodeToObj(stream, polygons, vertexIndices);
//        }
//
//        return;
//    }
//
//    stream << "o node_" << mAABBMax.x << "\n";
//
//    for (uint32_t i = 0; i < mLeafData->mPolygonIndices.size(); i++) {
//        const UNavPolygon& poly = polygons[mLeafData->mPolygonIndices[i]];
//
//        stream << "f ";
//        for (uint32_t j = 0; j < poly.mVertexCount; j++) {
//            stream << vertexIndices[poly.mFirstVertexIndex + j] + 1 << " ";
//        }
//        stream << "\n";
//    }
//}
