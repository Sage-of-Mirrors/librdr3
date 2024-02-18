#include "ynv/navmeshdata.hpp"
#include "ynv/navattribute.hpp"

#include <bstream.h>

#include <sstream>

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

void UNavmeshData::ReadUnk2Data(bStream::CStream* stream, UNavAttribute* attribute) {
    size_t streamPos = stream->tell();
    stream->seek(attribute->mDescriptorsOffset);

    for (uint32_t i = 0; i < attribute->mPoolCount; i++) {
        uint64_t dataOffset = stream->readUInt64() & 0x0FFFFFFF;
        uint32_t unk2Count = stream->readUInt32();
        uint32_t unkValue = stream->readUInt32();

        size_t localStreamPos = stream->tell();
        stream->seek(dataOffset);

        std::vector<UNavUnk2> localUnk2s;
        for (uint32_t j = 0; j < unk2Count; j++) {
            UNavUnk2 unk2;
            unk2.m0000 = stream->readUInt32();
            unk2.m0004 = stream->readUInt32();

            localUnk2s.push_back(unk2);
        }

        mUnk2.push_back(localUnk2s);
        stream->seek(localStreamPos);
    }

    stream->seek(streamPos);
}

void UNavmeshData::ReadUnk3Data(bStream::CStream* stream, UNavAttribute* attribute) {
    size_t streamPos = stream->tell();
    stream->seek(attribute->mDescriptorsOffset);

    for (uint32_t i = 0; i < attribute->mPoolCount; i++) {
        uint64_t dataOffset = stream->readUInt64() & 0x0FFFFFFF;
        uint32_t unk3Count = stream->readUInt32();
        uint32_t unkValue = stream->readUInt32();

        size_t localStreamPos = stream->tell();
        stream->seek(dataOffset);
        
        std::vector<UNavUnk3> localUnk3s;
        for (uint32_t j = 0; j < unk3Count; j++) {
            UNavUnk3 unk3;

            unk3.m0000 = stream->readUInt16();
            unk3.m0002 = stream->readUInt16();
            unk3.m0004 = stream->readUInt32();

            unk3.m0008 = stream->readUInt64();
            unk3.m0010 = stream->readUInt64();

            unk3.mBoundsMin.x = stream->readInt16() * 0.25f;
            unk3.mBoundsMin.y = stream->readInt16() * 0.25f;
            unk3.mBoundsMin.z = stream->readInt16() * 0.25f;
            unk3.mBoundsMax.x = stream->readInt16() * 0.25f;
            unk3.mBoundsMax.y = stream->readInt16() * 0.25f;
            unk3.mBoundsMax.z = stream->readInt16() * 0.25f;

            unk3.m0024 = stream->readUInt16();
            unk3.m0026 = stream->readUInt16();
            unk3.m0028 = stream->readUInt16();
            unk3.m002A = stream->readUInt16();
            unk3.m002C = stream->readUInt16();

            unk3.m002E = stream->readUInt8();
            unk3.m002F = stream->readUInt8();

            unk3.m0030 = stream->readUInt16();

            unk3.m0032 = stream->readUInt16();
            unk3.m0034 = stream->readUInt16();
            unk3.m0036 = stream->readUInt16();

            localUnk3s.push_back(unk3);
        }

        mUnk3.push_back(localUnk3s);
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
            UVector3 f = (v * (mBoundsMax.xyz() - mBoundsMin.xyz())) + mBoundsMin.xyz();
            stream << "v " << f.x << " " << f.y << " " << f.z << "\n";
        }
    }

    stream << "\n";

    //for (int j = 0; j < mIndices.size(); j++) {
    //    stream << "o indices_" << j << "\n";
    //    for (int i = 0; i < mIndices[j].size(); i += 3) {
    //        stream << "f " << mIndices[j][i] + 1 << " " << mIndices[j][i + 1] + 1 << " " << mIndices[j][i + 2] + 1 << "\n";
    //    }
    //}

    for (int i = 0; i < mUnk3.size(); i++) {
        for (const UNavUnk3& n : mUnk3[i]) {
            //stream << "o grp_" << n.m0004 << "\n";
            stream << "f ";

            int cnt = n.m0002 >> 5;
            for (int j = 0; j < cnt; j++) {
                stream << mIndices[n.m0004 + j] + 1 << " ";
            }
            stream << "\n";
        }
    }
    
    //for (int i = 0; i < mUnk3.size(); i++) {
    //    for (const UNavUnk3& n : mUnk3[i]) {
    //        stream << "v " << n.mBoundsMin.x << " " << n.mBoundsMin.y << " " << n.mBoundsMin.z << "\n";
    //        stream << "v " << n.mBoundsMax.x << " " << n.mBoundsMax.y << " " << n.mBoundsMax.z << "\n";
    //    }
    //}

    //stream << "v " << mBoundsMin.x << " " << mBoundsMin.y << " " << mBoundsMin.z << "\n";
    //stream << "v " << mBoundsMax.x << " " << mBoundsMax.y << " " << mBoundsMax.z << "\n";

    //stream << "v " << mBoundsMax.x << " " << mBoundsMin.y << " " << mBoundsMin.z << "\n";
    //stream << "v " << mBoundsMax.x << " " << mBoundsMin.y << " " << mBoundsMax.z << "\n";

    //stream << "v " << mBoundsMax.x << " " << mBoundsMax.y << " " << mBoundsMin.z << "\n";
    //stream << "v " << mBoundsMin.x << " " << mBoundsMin.y << " " << mBoundsMax.z << "\n";

    //stream << "v " << mBoundsMin.x << " " << mBoundsMax.y << " " << mBoundsMin.z << "\n";
    //stream << "v " << mBoundsMin.x << " " << mBoundsMax.y << " " << mBoundsMax.z << "\n";

    bStream::CFileStream objStream(objFile, bStream::Out);
    objStream.writeString(stream.str());
}

void UNavmeshData::Deserialize(bStream::CStream* stream) {
    if (stream == nullptr) {
        return;
    }

    size_t streamPos = 0;
    mVTable = stream->readUInt64();

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

    mMatrix0020.r0.x = stream->readFloat();
    mMatrix0020.r0.y = stream->readFloat();
    mMatrix0020.r0.z = stream->readFloat();
    mMatrix0020.r0.w = stream->readFloat();

    mMatrix0020.r1.x = stream->readFloat();
    mMatrix0020.r1.y = stream->readFloat();
    mMatrix0020.r1.z = stream->readFloat();
    mMatrix0020.r1.w = stream->readFloat();

    mMatrix0020.r2.x = stream->readFloat();
    mMatrix0020.r2.y = stream->readFloat();
    mMatrix0020.r2.z = stream->readFloat();
    mMatrix0020.r2.w = stream->readFloat();

    m0050 = stream->readUInt64();
    m0058 = stream->readUInt64();

    m0060.x = stream->readFloat();
    m0060.y = stream->readFloat();
    m0060.z = stream->readFloat();
    m0060.w = stream->readFloat();

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

    mTriangleCount = stream->readUInt32();

    uint32_t adjacentMeshCount = stream->readUInt32();
    for (int i = 0; i < 32; i++) {
        uint32_t adjacentMeshId = stream->readUInt32();

        if (adjacentMeshId != 0) {
            mAdjacentMeshIds.push_back(adjacentMeshId);
        }
    }

    uint64_t unk3AttributeOffset = stream->readUInt64() & 0x0FFFFFFF;
    streamPos = stream->tell();
    stream->seek(unk3AttributeOffset);
    UNavAttribute unk3Attribute;
    unk3Attribute.Deserialize(stream);
    stream->seek(streamPos);

    uint64_t boundsOffset = stream->readUInt64() & 0x0FFFFFFF;
    if (boundsOffset != 0) {
        streamPos = stream->tell();
        stream->seek(boundsOffset);

        mBoundsMin.x = stream->readFloat();
        mBoundsMin.y = stream->readFloat();
        mBoundsMin.z = stream->readFloat();
        mBoundsMin.w = stream->readFloat();

        mBoundsMax.x = stream->readFloat();
        mBoundsMax.y = stream->readFloat();
        mBoundsMax.z = stream->readFloat();
        mBoundsMax.w = stream->readFloat();

        stream->seek(streamPos);
    }

    uint64_t unk4Offset = stream->readUInt64() & 0x0FFFFFFF;
    uint64_t unk5Offset = stream->readUInt64() & 0x0FFFFFFF;

    m0138 = stream->readUInt32();
    m013C = stream->readUInt32();

    mNavMeshIndex = stream->readUInt32();
    mSectorY = (mNavMeshIndex / 215);
    mSectorX = mNavMeshIndex - (mSectorY * 215);
    mSectorY *= 3;
    mSectorX *= 3;

    m0144 = stream->readUInt32();
    m0148 = stream->readUInt32();

    mUnk4Count = stream->readUInt32();
    mUnk5Count = stream->readUInt32();

    ReadVertexData(stream, &vtxAttribute);
    ReadIndexData(stream, &indexAttribute);
    ReadUnk2Data(stream, &unk2Attribute);
    ReadUnk3Data(stream, &unk3Attribute);

    if (unk4Offset != 0) {
        ReadUnk4Data(stream, unk4Offset, mUnk4Count);
    }
    if (unk5Offset != 0) {
        ReadUnk5Data(stream, unk5Offset, mUnk5Count);
    }

    Debug_DumpToObj("D:\\RedDead\\Navmesh\\test.obj");
}

void UNavmeshData::Serialize(bStream::CMemoryStream* stream) {

}
