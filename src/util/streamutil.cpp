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

void UStreamUtil::SerializeMatrix3x4(bStream::CStream* stream, const UMatrix3x4& mat) {
    stream->writeFloat(mat.r0.x);
    stream->writeFloat(mat.r0.y);
    stream->writeFloat(mat.r0.z);
    stream->writeFloat(mat.r0.w);

    stream->writeFloat(mat.r1.x);
    stream->writeFloat(mat.r1.y);
    stream->writeFloat(mat.r1.z);
    stream->writeFloat(mat.r1.w);

    stream->writeFloat(mat.r2.x);
    stream->writeFloat(mat.r2.y);
    stream->writeFloat(mat.r2.z);
    stream->writeFloat(mat.r2.w);
}

void UStreamUtil::DeserializeVector3(bStream::CStream* stream, UVector3& vec) {
    vec.x = stream->readFloat();
    vec.y = stream->readFloat();
    vec.z = stream->readFloat();
}

void UStreamUtil::SerializeVector3(bStream::CStream* stream, const UVector3& vec) {
    stream->writeFloat(vec.x);
    stream->writeFloat(vec.y);
    stream->writeFloat(vec.z);
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

void UStreamUtil::SerializePtr64(bStream::CStream* stream, uint64_t offset) {
    stream->writeUInt64(offset | 0x50000000);
}

void UStreamUtil::PadStream(bStream::CStream* stream, uint32_t value) {
    size_t nextAligned = (stream->tell() + (value - 1)) & ~(value - 1);
    size_t delta = nextAligned - stream->tell();

    for (size_t i = 0; i < delta; i++) {
        stream->writeUInt8(0);
    }
}
