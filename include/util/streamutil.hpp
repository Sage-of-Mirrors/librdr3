#pragma once

#include "types.h"

struct Matrix3x4;
struct Vector3;
struct Vector4;

namespace StreamUtil {
    void DeserializeMatrix3x4(bStream::CStream* stream, Matrix3x4& mat);
    void SerializeMatrix3x4(bStream::CStream* stream, const Matrix3x4& mat);

    void DeserializeVector3(bStream::CStream* stream, Vector3& vec);
    void SerializeVector3(bStream::CStream* stream, const Vector3& vec);

    void DeserializeVector4(bStream::CStream* stream, Vector4& vec);

    uint64_t DeserializePtr64(bStream::CStream* stream);
    void SerializePtr64(bStream::CStream* stream, uint64_t offset);

    void PadStream(bStream::CStream* stream, uint32_t value);
}
