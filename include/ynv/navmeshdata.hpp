#pragma once

#include "types.h"
#include "math.hpp"
#include "ynv/splitarray.hpp"

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

class UBlockMap;

namespace UNavmesh {
    struct UNavQuadtreeNode;

    // Describes the edge that a vertex belongs to.
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

    // A polygon in the mesh, consisting of vertex/edge indices and other data.
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

    struct UNavLink {
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

    void DeserializeVertex(bStream::CStream* stream, std::shared_ptr<UVector3> vector);
    void DeserializeVertexIndex(bStream::CStream* stream, std::shared_ptr<uint16_t> index);
    void DeserializeAdjPolygonData(bStream::CStream* stream, std::shared_ptr<UNavAdjacentPolyData> data);
    void DeserializePolygon(bStream::CStream* stream, std::shared_ptr<UNavPolygon> poly);
    void DeserializeLink(bStream::CStream* stream, std::shared_ptr<UNavLink> link);

    enum ENavMeshFlags {
        VertexDataCompressed = 1 << 0,
        HasLinks = 1 << 1,
        IsDynamic = 1 << 2,
        HasWater = 1 << 3,
        AddedAsDLC = 1 << 4,
        DLCSwappable = 1 << 5
    };

    class UNavmeshData {
                                                             // 0x0000 is for the vtable pointer, loaded at runtime
        UBlockMap* mBlockMap;                                // 0x0008

        uint32_t mFlags;                                     // 0x0010
        uint32_t mFileVersion;                               // 0x0014

        uint64_t m0018;                                      // 0x0018

        UMatrix3x4 mTransformMatrix;                         // 0x0020
                                                             // 0x0050 is padding for transform matrix
                                                             // 0x0058 is padding for transform matrix

        UVector4 mMeshExtents;                               // 0x0060

        USplitArray<UVector3, VERTEX_BUCKET_SIZE> mVertices; // Offset at 0x0070
                                                             // 0x0078 is the offset for uncompressed vertices, but it's never used - all vertices are compressed

        USplitArray<uint16_t, VERTEX_INDEX_BUCKET_SIZE> mVertexIndices;           // Offset at 0x0080
        USplitArray<UNavAdjacentPolyData, ADJ_POLYGONS_BUCKET_SIZE> mAdjPolygons; // Offset at 0x0088
        
        uint32_t mIndexCount;                                // 0x0090
        uint32_t mAdjacentMeshIds[0x20] { };                 // Count at 0x0094, array from 0x0098 to 0x0118

        USplitArray<UNavPolygon, POLYGON_BUCKET_SIZE> mPolygons; // Offset at 0x0118
        std::shared_ptr<UNavQuadtreeNode> mQuadtreeRoot;         // Offset at 0x0120

        std::vector<std::shared_ptr<UNavLink>> mLinks;       // Offset at 0x0128, only present if HasLinks flag is set
        std::vector<uint16_t> mUnk5;                         // Offset at 0x0130, only present if HasLinks flag is set

        uint32_t mVertexCount;                               // 0x0138
        uint32_t mPolygonCount;                              // 0x013C
        uint16_t mSectorIndex;                               // 0x0140

        uint32_t mDataSize;                                  // 0x0144, size of all data that ISN'T SplitArray metadata
                                                             // 0x0148, called "numChildBounds" but isn't used in RDR3?

        uint32_t mLinkCount;                                 // 0x014C
        uint32_t mUnk5Count;                                 // 0x0150
                                                             // 0x0154 is padding?
                                                             // 0x0158 is padding?
                                                             // 0x015C is padding?

        uint32_t mUnk6;                                      // 0x0160
                                                             // 0x0164 is padding?
                                                             // 0x0168 is padding?
                                                             // 0x016C is padding?

        uint32_t mSectorX;                                   // Calculated from mSectorIndex
        uint32_t mSectorY;                                   // Calculated from mSectorIndex

        void ProcessData();

        bool GetFlag(const uint8_t flag) const { return (mFlags & flag) != 0; }
        void SetFlag(const uint8_t flag) { mFlags |= flag; }

    public:
        UNavmeshData();
        virtual ~UNavmeshData();

        void Deserialize(bStream::CStream* stream);
        void Serialize(bStream::CMemoryStream* stream);

        void Debug_DumpToObj(std::string objFile);
        //void Debug_DumpQuadtreeToObj(std::string objFile);
    };
}
