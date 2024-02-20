#pragma once

#include "types.h"

struct UMatrix3x4;
struct UVector3;
struct UVector4;

namespace UStreamUtil {
    void DeserializeMatrix3x4(bStream::CStream* stream, UMatrix3x4& mat);
    void SerializeMatrix3x4(bStream::CStream* stream, const UMatrix3x4& mat);

    void DeserializeVector3(bStream::CStream* stream, UVector3& vec);
    void SerializeVector3(bStream::CStream* stream, const UVector3& vec);

    void DeserializeVector4(bStream::CStream* stream, UVector4& vec);

    uint64_t DeserializePtr64(bStream::CStream* stream);
    void SerializePtr64(bStream::CStream* stream, uint64_t offset);

    void PadStream(bStream::CStream* stream, uint32_t value);
}
