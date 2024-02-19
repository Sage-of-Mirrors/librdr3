#pragma once

#include "types.h"
#include "math.hpp"

#include <vector>
#include <string>
#include <memory>
#include <sstream>

class UBlockMap;
struct UNavAttribute;

struct UNavAdjacentPolyData {
    uint8_t mSectorIndex;
    bool bUnk;
    uint16_t mPolygonIndex;
    uint8_t mAdjacencyType;
    float mSpaceAroundVertex;
    float mSpaceBeyondEdge;

    uint8_t mSectorIndex2;
    bool bUnk2;
    uint16_t mPolygonIndex2;
    bool bAdjacencyDisabled;
    bool bProvidesCover;
    bool bHighDrop;
    bool bExternal;
};

enum EPolygonFlags {
    Small = 1 << 0,
    Large = 1 << 1,
    Paved = 1 << 2,
    Sheltered = 1 << 3,
    // unused  = 1 << 4,
    // unused  = 1 << 5,
    Steep = 1 << 6,
    Water = 1 << 7
};

struct UNavPolygon {
    uint16_t mFlags;            // 0x00
    uint16_t mVertexCount;      // 0x02, at mask 0x01F0
    uint32_t mFirstVertexIndex; // 0x04

    uint64_t m0008;             // 0x08
    uint64_t m0010;             // 0x10

    UVector3 mBoundsMin;        // 0x18
    UVector3 mBoundsMax;        // 0x1E

    uint16_t m0024;             // 0x24
    uint16_t m0026;             // 0x26
    uint16_t m0028;             // 0x28
    uint16_t m002A;             // 0x2A
    uint16_t m002C;             // 0x2C

    uint8_t m002E;              // 0x2E
    uint8_t m002F;              // 0x2F

    uint16_t m0030;             // 0x30

    uint16_t m0032;             // 0x32
    uint16_t m0034;             // 0x34
    uint16_t m0036;             // 0x36
};

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

    void Debug_DumpQuadtreeNodeToObj(std::stringstream& stream, const std::vector<UNavPolygon>& polygons, const std::vector<uint16_t>& vertexIndices);
};

struct UNavUnk4 {
    uint16_t m0000; // m0000

    UVector3 m0002; // m0002
    UVector3 m0008; // m0008

    uint16_t m000E; // m000E
    uint16_t m0010; // m0010
    uint16_t m0012; // m0012
    uint16_t m0014; // m0014

    uint8_t m0016;  // m0016
    uint8_t m0017;  // m0017
    uint8_t m0018;  // m0018
    uint8_t m0019;  // m0019

    uint16_t m001A; // m001A
    uint32_t m001C; // m001C
};

enum ENavMeshFlags {
    VertexDataCompressed = 1 << 0,
    HasSpecialLinks      = 1 << 1,
    IsDynamic            = 1 << 2,
    HasWater             = 1 << 3,
    AddedAsDLC           = 1 << 4,
    DLCSwappable         = 1 << 5
};

class UNavmeshData {
    uint64_t mVTablePtr;                             // 0x0000
    UBlockMap* mBlockMap;                            // 0x0008

    uint32_t mFlags;                                 // 0x0010
    uint32_t mFileVersion;                           // 0x0014

    uint64_t m0018;                                  // 0x0018

    UMatrix3x4 mTransformMatrix;                     // 0x0020
    uint64_t m0050;                                  // 0x0050
    uint64_t m0058;                                  // 0x0058

    UVector4 mMeshExtents;                           // 0x0060

    std::vector<std::vector<UVector3>> mVertices;    // Attribute offset at 0x0070

    uint64_t m0078;                                  // 0x0078

    std::vector<uint16_t> mIndices;                  // Attribute offset at 0x0080
    std::vector<UNavAdjacentPolyData> mAdjPolygons;  // Attribute offset at 0x0088
    uint32_t mIndexCount;                            // 0x0090

    std::vector<uint32_t> mAdjacentMeshIds;          // Count at 0x0094, array from 0x0098 to 0x0118

    std::vector<UNavPolygon> mPolygons;              // Attribute offset at 0x0118
    std::shared_ptr<UNavQuadtreeNode> mQuadtreeRoot; // Pointer at 0x0120

    std::vector<UNavUnk4> mUnk4;                     // Offset at 0x0128, only present if HasSpecialLinks flag is set
    std::vector<uint16_t> mUnk5;                     // Offset at 0x0130, only present if HasSpecialLinks flag is set

    uint32_t mVertexCount;                           // 0x0138
    uint32_t mPolygonCount;                          // 0x013C
    uint16_t mSectorIndex;                           // 0x0140

    uint32_t m0144;                                  // 0x0144
    uint32_t m0148;                                  // 0x0148

    uint32_t mUnk4Count;                             // 0x014C
    uint32_t mUnk5Count;                             // 0x0150

    uint32_t mSectorX;                               // Calculated from mNavMeshIndex
    uint32_t mSectorY;                               // Calculated from mNavMeshIndex

    void ReadVertexData(bStream::CStream* stream, UNavAttribute* attribute);
    void ReadIndexData(bStream::CStream* stream, UNavAttribute* attribute);
    void ReadPolyAdjacencyData(bStream::CStream* stream, UNavAttribute* attribute);
    void ReadPolygonData(bStream::CStream* stream, UNavAttribute* attribute);
    
    void ReadUnk4Data(bStream::CStream* stream, uint64_t offset, uint32_t count);
    void ReadUnk5Data(bStream::CStream* stream, uint64_t offset, uint32_t count);

    bool GetFlag(const uint8_t flag) const { return (mFlags & flag) != 0; }
    void SetFlag(const uint8_t flag) { mFlags |= flag; }

    void Debug_DumpToObj(std::string objFile);

public:
    UNavmeshData();
    virtual ~UNavmeshData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CMemoryStream* stream);

    void Debug_DumpQuadtreeToObj(std::string objFile);
};
