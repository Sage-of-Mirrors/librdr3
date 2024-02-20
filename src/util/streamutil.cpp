#include "util/streamutil.hpp"
#include "math.hpp"

#include <bstream.h>

void UStreamUtil::DeserializeMatrix3x4(bStream::CStream* stream, UMatrix3x4& mat) {
    mat.r0.x = stream->readFloat();
    mat.r0.y = stream->readFloat();
    mat.r0.z = stream->readFloat();
    mat.r0.w = stream->readFloat();

    mat.r1.x = stream->readFloat();
    mat.r1.y = stream->readFloat();
    mat.r1.z = stream->readFloat();
    mat.r1.w = stream->readFloat();

    mat.r2.x = stream->readFloat();
    mat.r2.y = stream->readFloat();
    mat.r2.z = stream->readFloat();
    mat.r2.w = stream->readFloat();
}

void UStreamUtil::DeserializeVector3(bStream::CStream* stream, UVector3& vec) {
    vec.x = stream->readFloat();
    vec.y = stream->readFloat();
    vec.z = stream->readFloat();
}

void UStreamUtil::DeserializeVector4(bStream::CStream* stream, UVector4& vec) {
    vec.x = stream->readFloat();
    vec.y = stream->readFloat();
    vec.z = stream->readFloat();
    vec.w = stream->readFloat();
}

uint64_t UStreamUtil::DeserializePtr64(bStream::CStream* stream) {
    return stream->readUInt64() & 0x0FFFFFFF;
}
