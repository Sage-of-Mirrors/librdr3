#include "ynv/navattribute.hpp"

#include <bstream.h>

void UNavAttribute::Serialize(bStream::CStream* stream) {

}

void UNavAttribute::Deserialize(bStream::CStream* stream) {
    m0000 = stream->readFloat();
    m0004 = stream->readUInt32();

    mCount = stream->readUInt32();
    m000C = stream->readUInt32();

    uint64_t desc1Offset = stream->readUInt64() & 0x0FFFFFFF;
    uint64_t desc2Offset = stream->readUInt64() & 0x0FFFFFFF;

    m0020 = stream->readUInt32();
    m0024 = stream->readUInt32();
    m0028 = stream->readUInt32();
    m002C = stream->readUInt32();

    size_t streamPos = stream->tell();

    stream->seek(desc1Offset);
    mDescriptor.mDataOffset = stream->readUInt64() & 0x0FFFFFFF;
    mDescriptor.mCount = stream->readUInt32();
    mDescriptor.m000C = stream->readUInt32();

    stream->seek(desc2Offset);
    mSecondaryDescriptor.mDataOffset = stream->readUInt64() & 0x0FFFFFFF;
    mSecondaryDescriptor.mCount = stream->readUInt32();
    mSecondaryDescriptor.m000C = stream->readUInt32();

    stream->seek(streamPos);
}
