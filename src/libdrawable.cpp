#include "libdrawable.hpp"
#include "bstream.h"

#include "drawable.hpp"
#include "drawabledata.hpp"

#include "drawabledictionary.hpp"
#include "drawabledictionarydata.hpp"

UDrawable* ImportYdr(std::string filePath) {
    bStream::CFileStream stream(filePath);
    if (stream.peekUInt32(0) != 0) {
        return nullptr;
    }

    UDrawableData data;
    data.Deserialize(&stream);

    return data.GetDrawable();
}

UDrawableDictionary* ImportYdd(std::string filePath) {
    bStream::CFileStream stream(filePath);
    if (stream.peekUInt32(0) != 0) {
        return nullptr;
    }

    UDrawableDictionaryData data;
    data.Deserialize(&stream);

    return data.GetDrawableDictionary();
}

bool ExportYdr(std::string filePath, UDrawable* data) {
    // Set up drawable data for serialization
    UDrawableData* d = new UDrawableData();
    d->SetDrawable(data);

    // Serialize drawable data to memory stream and dispose of it
    bStream::CMemoryStream drawableData(0xD0, bStream::Little, bStream::Out);
    d->Serialize(&drawableData);
    delete d;

    // Write serialized data to file
    bStream::CFileStream fileStream(filePath, bStream::Out);
    fileStream.writeBytesTo(drawableData.getBuffer(), drawableData.getSize());

    return true;
}
