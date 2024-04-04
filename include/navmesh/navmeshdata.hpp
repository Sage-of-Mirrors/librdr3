#pragma once

#include "types.h"
#include "util/math.hpp"
#include "splitarray.hpp"

#include <vector>
#include <string>
#include <memory>
#include <sstream>

constexpr uint32_t RDR3_NAVMESH_VERSION     = 0x00010013;
constexpr uint32_t ADJACENT_NAVMESHES_MAX   = 0x00000020;

constexpr uint32_t VERTEX_BUCKET_SIZE       = 0x00000AAA;
constexpr uint32_t VERTEX_INDEX_BUCKET_SIZE = 0x00002000;
constexpr uint32_t ADJ_POLYGONS_BUCKET_SIZE = 0x00000800;
constexpr uint32_t POLYGON_BUCKET_SIZE      = 0x00000124;

class CBlockMap;
struct CNavQuadtreeNode;

// Describes the edge that a vertex belongs to.
struct CNavAdjacentPolyData {
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
    SMALL = 1 << 0,
    LARGE = 1 << 1,
    PAVED = 1 << 2,
    SHELTERED = 1 << 3,
    // unused  = 1 << 4,
    // unused  = 1 << 5,
    STEEP = 1 << 6,
    WATER = 1 << 7
};

struct CNavPolygonInfo1 {
    uint8_t mFlags;
    uint8_t mVertexCount;
    uint8_t mPointType;
};

struct CNavPolygonInfo2 {
    uint16_t mFirstVertexIndex;
    uint16_t mNavMeshIndex;
};

struct CNavPolygonInfo3 {
    uint16_t mAudioProperties;
    bool bDebug;
    bool bNearVehicle;
    bool bInterior;
    bool bIsolated;
    bool bZeroStitchDLC;
    bool bNetworkSpawnCandidate;
    bool bIsRoad;
    bool bOnEdgeOfMesh;
    bool bIsTrainTrack;
    bool bIsShallowWater;
    uint8_t mPedDensity;
};

// A polygon in the mesh, consisting of vertex/edge indices and other data.
struct CNavPolygon {
    CNavPolygonInfo1 mInfo1;    // 0x00, bitfield in a u32
    CNavPolygonInfo2 mInfo2;    // 0x04, bitfield in a u32
                                // 0x08, runtime fields not filled in file
                                // 0x10, runtime fields not filled in file

    Vector3 mBoundsMin;         // 0x18, interleaved with max
    Vector3 mBoundsMax;         // 0x1E, interleaved with min

    CNavPolygonInfo3 mInfo3;    // 0x24, bitfield in a u32

    float mCentroidX;           // 0x28, stored as a compressed u8
    float mCentroidY;           // 0x29, stored as a compressed u8

    uint8_t m002A;              // 0x2A
    uint8_t m002B;              // 0x2B
    uint8_t m002C;              // 0x2C
    uint8_t m002D;              // 0x2D
    uint8_t m002E;              // 0x2E
    uint8_t m002F;              // 0x2F

    uint16_t m0030;             // 0x30

    uint16_t m0032;             // 0x32
    uint16_t m0034;             // 0x34
    uint16_t m0036;             // 0x36
};

struct CNavLink {
    uint16_t m0000; // m0000

    Vector3 m0002;  // m0002
    Vector3 m0008;  // m0008

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
    VTX_DATA_COMPRESSED = 1 << 0,
    HAS_LINKS = 1 << 1,
    IS_DYNAMIC = 1 << 2,
    HAS_WATER = 1 << 3,
    IS_DLC = 1 << 4,
    DLC_SWAPPABLE = 1 << 5
};

class CNavmeshData {
                                                         // 0x0000 is for the vtable pointer, loaded at runtime
    CBlockMap* mBlockMap;                                // 0x0008

    uint32_t mFlags;                                     // 0x0010
    uint32_t mFileVersion;                               // 0x0014

    uint64_t m0018;                                      // 0x0018

    Matrix3x4 mTransformMatrix;                          // 0x0020
                                                         // 0x0050 is padding for transform matrix
                                                         // 0x0058 is padding for transform matrix

    Vector3 mMeshExtents;                                // 0x0060

    TSplitArray<Vector3, VERTEX_BUCKET_SIZE> mVertices;  // Offset at 0x0070
                                                         // 0x0078 is the offset for uncompressed vertices, but it's never used - all vertices are compressed

    TSplitArray<uint16_t, VERTEX_INDEX_BUCKET_SIZE> mVertexIndices;           // Offset at 0x0080
    TSplitArray<CNavAdjacentPolyData, ADJ_POLYGONS_BUCKET_SIZE> mAdjPolygons; // Offset at 0x0088
        
    uint32_t mIndexCount;                                // 0x0090
    uint32_t mAdjacentMeshIds[0x20] { };                 // Count at 0x0094, array from 0x0098 to 0x0118

    TSplitArray<CNavPolygon, POLYGON_BUCKET_SIZE> mPolygons; // Offset at 0x0118
    std::shared_ptr<CNavQuadtreeNode> mQuadtreeRoot;         // Offset at 0x0120

    shared_vector<CNavLink> mLinks;                      // Offset at 0x0128, only present if HasLinks flag is set
    std::vector<uint16_t> mLinkIndices;                  // Offset at 0x0130, only present if HasLinks flag is set

    uint32_t mVertexCount;                               // 0x0138
    uint32_t mPolygonCount;                              // 0x013C
    uint16_t mSectorIndex;                               // 0x0140

    uint32_t mDataSize;                                  // 0x0144, size of all data that ISN'T SplitArray metadata
                                                         // 0x0148, called "numChildBounds" but isn't used in RDR3?

    uint32_t mLinkCount;                                 // 0x014C
    uint32_t mLinkIndexCount;                            // 0x0150
                                                         // 0x0154 is padding
                                                         // 0x0158 is padding
                                                         // 0x015C is padding

    uint32_t mBuildId;                                   // 0x0160
                                                         // 0x0164 is padding
                                                         // 0x0168 is padding
                                                         // 0x016C is padding

    uint32_t mSectorX;                                   // Calculated from mSectorIndex
    uint32_t mSectorY;                                   // Calculated from mSectorIndex

    void ProcessData();

    bool GetFlag(const uint8_t flag) const { return (mFlags & flag) != 0; }
    void SetFlag(const uint8_t flag) { mFlags |= flag; }

public:
    CNavmeshData();
    virtual ~CNavmeshData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    void GetVertices(float*& vtxData, uint32_t*& indexData, uint32_t& vtxCount, uint32_t& indexCount);
    void GetIndicesForPolys(uint32_t*& data, uint32_t& indexCount);

    void Debug_DumpToObj(std::string objFile);
    //void Debug_DumpQuadtreeToObj(std::string objFile);
};
