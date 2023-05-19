#include "blockmap.h"
#include "stream.h"

void DeserializeBlockMap(UStream* stream, UBlockMap* dst) {
    dst->m0000 = UStreamReadU32(stream);
    dst->m0004 = UStreamReadU32(stream);

    dst->mVirtualPageCount = UStreamReadU8(stream);
    dst->mPhysicalPageCount = UStreamReadU8(stream);
    dst->m000A = UStreamReadU16(stream);

    dst->m000C = UStreamReadU32(stream);
    dst->m0010 = UStreamReadU32(stream);
    dst->m0014 = UStreamReadU32(stream);
    dst->m0018 = UStreamReadU32(stream);
    dst->m001C = UStreamReadU32(stream);
}

void SerializeBlockMap(UStream* stream, UBlockMap* src) {
    UStreamWriteU32(stream, src->m0000);
    UStreamWriteU32(stream, src->m0004);

    UStreamWriteU8(stream, src->mVirtualPageCount);
    UStreamWriteU8(stream, src->mPhysicalPageCount);
    UStreamWriteU16(stream, src->m000A);

    UStreamWriteU32(stream, src->m000C);
    UStreamWriteU32(stream, src->m0010);
    UStreamWriteU32(stream, src->m0014);
    UStreamWriteU32(stream, src->m0018);
    UStreamWriteU32(stream, src->m001C);
}
