#pragma once

#include "types.h"

#include <vector>

struct UDrawableDictionary;
struct UDrawable;

class UDrawableData;

class UDrawableDictionaryData {
    uint64_t mVTable; // 0x00

    class UBlockMap* mBlockMap; // 0x08

    uint64_t m0010;
    uint64_t m0018;

    std::vector<uint32_t> mHashes;
    uint32_t m002C;

    std::vector<UDrawableData*> mDrawables;
    uint32_t m003C;

public:
    UDrawableDictionaryData();
    virtual ~UDrawableDictionaryData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    UDrawableDictionary* GetDrawableDictionary();
};
