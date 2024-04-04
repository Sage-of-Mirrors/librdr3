#pragma once

#include "types.h"
#include "util/math.hpp"

#include <vector>
#include <memory>

struct CNavQuadtreeLeafData {
    uint64_t mRuntimePtr;                  // 0x00

    std::vector<uint16_t> mPolygonIndices; // Pointer at 0x08
    std::vector<uint32_t> mBounds;         // ?, Pointer at 0x10?

    void Deserialize(bStream::CStream* stream);
};

struct CNavQuadtreeNode {
    Vector3 mBoundsMin;                             // 0x00
    Vector3 mBoundsMax;                             // 0x10

    Vector3 mAABBMin;                               // 0x20, interleaved with max
    Vector3 mAABBMax;                               // 0x20, interleaved with min

    std::shared_ptr<CNavQuadtreeLeafData> mLeafData; // Pointer at 0x2C
    std::shared_ptr<CNavQuadtreeNode> mChildren[4];  // Pointers begin at 0x30

    Vector3 mExtents;

    bool IsLeaf() const { return mLeafData != nullptr; }
    void Deserialize(bStream::CStream* stream);
    void Deserialize_Recursive(bStream::CStream* stream);

    //void Debug_DumpQuadtreeNodeToObj(std::stringstream& stream, const std::vector<UNavPolygon>& polygons, const std::vector<uint16_t>& vertexIndices);
};
