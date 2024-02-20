#pragma once

#include "types.h"
#include "math.hpp"

#include <vector>
#include <memory>

namespace UNavmesh {
    struct UNavQuadtreeLeafData {
        uint64_t mRuntimePtr;                  // 0x00

        std::vector<uint16_t> mPolygonIndices; // Pointer at 0x08
        std::vector<uint32_t> mBounds;         // ?, Pointer at 0x10?

        void Deserialize(bStream::CStream* stream);
    };

    struct UNavQuadtreeNode {
        UVector3 mBoundsMin;                             // 0x00
        UVector3 mBoundsMax;                             // 0x10

        UVector3 mAABBMin;                               // 0x20, interleaved with max
        UVector3 mAABBMax;                               // 0x20, interleaved with min

        std::shared_ptr<UNavQuadtreeLeafData> mLeafData; // Pointer at 0x2C
        std::shared_ptr<UNavQuadtreeNode> mChildren[4];  // Pointers begin at 0x30

        UVector3 mExtents;

        bool IsLeaf() const { return mLeafData != nullptr; }
        void Deserialize(bStream::CStream* stream);
        void Deserialize_Recursive(bStream::CStream* stream);

        //void Debug_DumpQuadtreeNodeToObj(std::stringstream& stream, const std::vector<UNavPolygon>& polygons, const std::vector<uint16_t>& vertexIndices);
    };
}