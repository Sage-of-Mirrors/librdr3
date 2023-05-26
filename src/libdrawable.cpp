#include "libdrawable.hpp"
#include "bstream.h"

#include "drawable.hpp"
#include "drawabledata.hpp"

#include "drawabledictionary.hpp"
#include "drawabledictionarydata.hpp"

UDrawable* ImportYdr(std::string filePath) {
    bStream::CFileStream stream(filePath);

    UDrawableData data;
    data.Deserialize(&stream);

    return data.GetDrawable();
}

bool ExportYdr(std::string filePath, UDrawable* data) {
    return true;
}

UDrawableDictionary* ImportYdd(std::string filePath) {
    bStream::CFileStream stream(filePath);

    UDrawableDictionaryData data;
    data.Deserialize(&stream);

    return data.GetDrawableDictionary();
}