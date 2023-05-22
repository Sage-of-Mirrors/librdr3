#include "libdrawable.hpp"
#include "bstream.h"
#include "drawable.hpp"
#include "drawabledata.hpp"

UDrawable* ImportYdr(std::string filePath) {
    bStream::CFileStream stream(filePath);

    UDrawableData data;
    data.Deserialize(&stream);

    return data.GetDrawable();
}

bool ExportYdr(std::string filePath, UDrawable* data) {
    return true;
}