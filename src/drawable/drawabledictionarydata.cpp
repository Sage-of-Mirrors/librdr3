#include "drawable/drawabledictionarydata.hpp"
#include "drawable/drawabledictionary.hpp"
#include "drawable/drawabledata.hpp"

#include "util/bstream.h"

CDrawableDictionaryData::CDrawableDictionaryData() {

}

CDrawableDictionaryData::~CDrawableDictionaryData() {
    mDrawables.clear();
}

void CDrawableDictionaryData::Deserialize(bStream::CStream* stream) {
    mVTable = stream->readUInt64();
    uint64_t blockMapPtr = stream->readUInt64() & 0x0FFFFFFF;

    m0010 = stream->readUInt64();
    m0018 = stream->readUInt64();

    uint64_t hashesPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint16_t hashesCount = stream->readUInt16();
    uint16_t hashesCapacity = stream->readUInt16();
    m002C = stream->readUInt32();

    uint64_t drawablesPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint16_t drawablesCount = stream->readUInt16();
    uint16_t drawablesCapacity = stream->readUInt16();
    m003C = stream->readUInt32();

    // Read hashes
    stream->seek(hashesPtr);
    for (int i = 0; i < hashesCount; i++) {
        mHashes.push_back(stream->readUInt32());
    }

    // Read drawables
    stream->seek(drawablesPtr);
    size_t streamPos = 0;

    for (int i = 0; i < drawablesCount; i++) {
        uint64_t curDrawablePtr = stream->readUInt64() & 0x0FFFFFFF;
        
        streamPos = stream->tell();
        stream->seek(curDrawablePtr);

        std::shared_ptr<CDrawableData> drawable = std::make_shared<CDrawableData>();
        drawable->Deserialize(stream);
        mDrawables.push_back(drawable);

        stream->seek(streamPos);
    }
}

void CDrawableDictionaryData::Serialize(bStream::CStream* stream) {

}

std::shared_ptr<CDrawableDictionary> CDrawableDictionaryData::GetDrawableDictionary() {
    std::shared_ptr<CDrawableDictionary> dict = std::make_shared<CDrawableDictionary>();

    for (int i = 0; i < mDrawables.size(); i++) {
        std::shared_ptr<CDrawable> drawable = mDrawables[i]->GetDrawable();
        drawable->DictionaryHash = mHashes[i];

        dict->Drawables.push_back(drawable);
    }

    return dict;
}
