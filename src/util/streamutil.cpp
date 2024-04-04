#include "util/streamutil.hpp"
#include "util/math.hpp"
#include "util/bstream.h"

void StreamUtil::DeserializeMatrix3x4(bStream::CStream* stream, Matrix3x4& mat) {
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

void StreamUtil::SerializeMatrix3x4(bStream::CStream* stream, const Matrix3x4& mat) {
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

void StreamUtil::DeserializeVector3(bStream::CStream* stream, Vector3& vec) {
    vec.x = stream->readFloat();
    vec.y = stream->readFloat();
    vec.z = stream->readFloat();
}

void StreamUtil::SerializeVector3(bStream::CStream* stream, const Vector3& vec) {
    stream->writeFloat(vec.x);
    stream->writeFloat(vec.y);
    stream->writeFloat(vec.z);
}

void StreamUtil::DeserializeVector4(bStream::CStream* stream, Vector4& vec) {
    vec.x = stream->readFloat();
    vec.y = stream->readFloat();
    vec.z = stream->readFloat();
    vec.w = stream->readFloat();
}

uint64_t StreamUtil::DeserializePtr64(bStream::CStream* stream) {
    return stream->readUInt64() & 0x0FFFFFFF;
}

void StreamUtil::SerializePtr64(bStream::CStream* stream, uint64_t offset) {
    stream->writeUInt64(offset | 0x50000000);
}

void StreamUtil::PadStream(bStream::CStream* stream, uint32_t value) {
    size_t nextAligned = (stream->tell() + (value - 1)) & ~(value - 1);
    size_t delta = nextAligned - stream->tell();

    for (size_t i = 0; i < delta; i++) {
        stream->writeUInt8(0);
    }
}
