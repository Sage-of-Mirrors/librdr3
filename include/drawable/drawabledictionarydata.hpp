#pragma once

#include "types.h"

#include <vector>
#include <memory>

struct CDrawableDictionary;
struct CDrawable;

class CDrawableData;

class CDrawableDictionaryData {
    uint64_t mVTable; // 0x00

    class UBlockMap* mBlockMap; // 0x08

    uint64_t m0010;
    uint64_t m0018;

    std::vector<uint32_t> mHashes;
    uint32_t m002C;

    shared_vector<CDrawableData> mDrawables;
    uint32_t m003C;

public:
    CDrawableDictionaryData();
    virtual ~CDrawableDictionaryData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    std::shared_ptr<CDrawableDictionary> GetDrawableDictionary();
};
