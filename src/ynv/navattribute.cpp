#include "ynv/navattribute.hpp"

#include <bstream.h>

void UNavAttribute::Serialize(bStream::CStream* stream) {

}

void UNavAttribute::Deserialize(bStream::CStream* stream) {
    m0000 = stream->readFloat();
    m0004 = stream->readUInt32();

    mTotalCount = stream->readUInt32();
    m000C = stream->readUInt32();

    mDescriptorsOffset = stream->readUInt64() & 0x0FFFFFFF;
    mUnkOffset = stream->readUInt64() & 0x0FFFFFFF;

    mPoolCount = stream->readUInt32();

    m0024 = stream->readUInt32();
    m0028 = stream->readUInt32();
    m002C = stream->readUInt32();
}
