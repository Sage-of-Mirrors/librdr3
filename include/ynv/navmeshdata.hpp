#pragma once

#include "types.h"
#include "math.hpp"

#include <vector>
#include <string>

class UBlockMap;
struct UNavAttribute;

struct UNavUnk2 {
    uint32_t m0000; // 0x00
    uint32_t m0004; // 0x00
};

struct UNavUnk3 {
    uint16_t m0000; // 0x00
    uint16_t m0002; // 0x02
    uint32_t m0004; // 0x04

    uint64_t m0008; // 0x08
    uint64_t m0010; // 0x10

    UVector3 mBoundsMin; // 0x18
    UVector3 mBoundsMax; // 0x1E

    uint16_t m0024; // 0x24
    uint16_t m0026; // 0x26
    uint16_t m0028; // 0x28
    uint16_t m002A; // 0x2A
    uint16_t m002C; // 0x2C

    uint8_t m002E; // 0x2E
    uint8_t m002F; // 0x2F

    uint16_t m0030; // 0x30

    uint16_t m0032; // 0x32
    uint16_t m0034; // 0x34
    uint16_t m0036; // 0x36
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
    uint64_t mVTable;       // 0x0000
    UBlockMap* mBlockMap;   // 0x0008

    uint32_t mFlags;        // 0x0010
    uint32_t mFileVersion;  // 0x0014

    uint64_t m0018;         // 0x0018

    UMatrix3x4 mMatrix0020; // 0x0020
    uint64_t m0050;         // 0x0050
    uint64_t m0058;         // 0x0058

    UVector4 m0060;         // 0x0060

    std::vector<std::vector<UVector3>> mVertices; // Attribute offset at 0x0070

    uint64_t m0078;         // 0x0078

    std::vector<uint16_t> mIndices;  // Attribute offset at 0x0080
    std::vector<UNavUnk2> mUnk2;     // Attribute offset at 0x0088

    uint32_t mTriangleCount;                // 0x0090
    std::vector<uint32_t> mAdjacentMeshIds; // Count at 0x0094, array from 0x0098 to 0x0118

    std::vector<UNavUnk3> mUnk3; // Attribute offset at 0x0118
    std::vector<UNavUnk3> mUnk3_2;

    UVector4 mBoundsMin;    // Offset at 0x0120
    UVector4 mBoundsMax;    // Offset at 0x0120

    std::vector<UNavUnk4> mUnk4; // Offset at 0x0128, sometimes nullptr
    std::vector<uint16_t> mUnk5; // Offset at 0x0130, sometimes nullptr

    uint32_t m0138;         // 0x0138
    uint32_t m013C;         // 0x013C

    uint16_t mNavMeshIndex;  // 0x0140

    uint32_t m0144;         // 0x0144
    uint32_t m0148;         // 0x0148

    uint32_t mUnk4Count;    // 0x014C
    uint32_t mUnk5Count;    // 0x0150

    uint32_t mSectorX; // Calculated from mNavMeshIndex
    uint32_t mSectorY; // Calculated from mNavMeshIndex

    void ReadVertexData(bStream::CStream* stream, UNavAttribute* attribute);
    void ReadIndexData(bStream::CStream* stream, UNavAttribute* attribute);
    void ReadUnk2Data(bStream::CStream* stream, UNavAttribute* attribute);
    void ReadUnk3Data(bStream::CStream* stream, UNavAttribute* attribute);
    
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
};
