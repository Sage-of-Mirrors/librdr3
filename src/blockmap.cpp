#include "blockmap.hpp"

#include "util/bstream.h"

void CBlockMap::Deserialize(bStream::CStream* stream) {
    m0000 = stream->readUInt32();
    m0004 = stream->readUInt32();

    mVirtualPageCount = stream->readUInt8();
    mPhysicalPageCount = stream->readUInt8();
    m000A = stream->readUInt16();

    m000C = stream->readUInt32();
    m0010 = stream->readUInt32();
    m0014 = stream->readUInt32();
    m0018 = stream->readUInt32();
    m001C = stream->readUInt32();
}

void CBlockMap::Serialize(bStream::CStream* stream) {
    stream->writeUInt32(m0000);
    stream->writeUInt32(m0004);

    stream->writeUInt8(mVirtualPageCount);
    stream->writeUInt8(mPhysicalPageCount);
    stream->writeUInt16(m000A);

    stream->writeUInt32(m000C);
    stream->writeUInt32(m0010);
    stream->writeUInt32(m0014);
    stream->writeUInt32(m0018);
    stream->writeUInt32(m001C);
}
