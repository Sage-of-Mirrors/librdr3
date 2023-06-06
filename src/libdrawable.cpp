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
    return true;
}
