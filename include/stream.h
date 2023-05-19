/* On loan from https://github.com/Astral-C/libgctools/blob/master/include/stream.h */
#ifndef LIBDRAWABLE_STREAM_H
#define LIBDRAWABLE_STREAM_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif 

    typedef struct UStream {
        const uint8_t* buffer;
        size_t position;
        size_t size;
        UStreamEndian endian;
        UStreamEndian systemOrder;
    } UStream;

    // Management Functions

    UStreamError UInitStream(UStream* stream, const void* buffer, size_t size, UStreamEndian endian);
    UStreamError UStreamSeek(UStream* stream, uint32_t pos, uint32_t from);

    // Reader Functions

    int8_t UStreamRead8(UStream* stream);
    uint8_t UStreamReadU8(UStream* stream);

    int16_t UStreamRead16(UStream* stream);
    uint16_t UStreamReadU16(UStream* stream);

    int32_t UStreamRead32(UStream* stream);
    uint32_t UStreamReadU32(UStream* stream);

    int64_t UStreamRead64(UStream* stream);
    uint64_t UStreamReadU64(UStream* stream);

    float UStreamReadFloat(UStream* stream);
    double UStreamReadDouble(UStream* stream);

    char* UStreamReadStr(UStream* stream, size_t size);

    // Writer Functions

    void UStreamWrite8(UStream* stream, int8_t r);
    void UStreamWriteU8(UStream* stream, uint8_t r);

    void UStreamWrite16(UStream* stream, int16_t r);
    void UStreamWriteU16(UStream* stream, uint16_t r);

    void UStreamWrite32(UStream* stream, int32_t r);
    void UStreamWriteU32(UStream* stream, uint32_t r);

    void UStreamWrite64(UStream* stream, int64_t r);
    void UStreamWriteU64(UStream* stream, uint64_t r);

    void UStreamWriteFloat(UStream* stream, float r);
    void UStreamWriteDouble(UStream* stream, double r);

    void UStreamWriteStr(UStream* stream, const char* str, size_t size);

#ifdef __cplusplus
}
#endif

#endif