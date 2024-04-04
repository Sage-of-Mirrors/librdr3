#include "librdr3.hpp"

#include "drawable/drawable.hpp"
#include "drawable/drawabledata.hpp"
#include "drawable/drawabledictionary.hpp"
#include "drawable/drawabledictionarydata.hpp"

#include "util/bstream.h"

std::shared_ptr<CDrawable> librdr3::ImportYdr(std::string filePath) {
    bStream::CFileStream stream(filePath);
    if (stream.peekUInt32(0) != 0) {
        return nullptr;
    }

    CDrawableData data;
    data.Deserialize(&stream);

    return data.GetDrawable();
}

std::shared_ptr<CDrawableDictionary> librdr3::ImportYdd(std::string filePath) {
    bStream::CFileStream stream(filePath);
    if (stream.peekUInt32(0) != 0) {
        return nullptr;
    }

    CDrawableDictionaryData data;
    data.Deserialize(&stream);

    return data.GetDrawableDictionary();
}

std::shared_ptr<CFragment> librdr3::ImportYft(std::string filePath) {
    bStream::CFileStream stream(filePath);
    if (stream.peekUInt32(0) != 0) {
        return nullptr;
    }
}

std::shared_ptr<CNavmeshData> librdr3::ImportYnv(std::string filePath) {
    bStream::CFileStream stream(filePath);
    if (stream.peekUInt32(0) != 0) {
        return nullptr;
    }

    std::shared_ptr<CNavmeshData> data = std::make_shared<CNavmeshData>();
    data->Deserialize(&stream);

    return data;
}

bool librdr3::ExportYdr(std::string filePath, std::shared_ptr<CDrawable> data) {
    // Set up drawable data for serialization
    CDrawableData* d = new CDrawableData();
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

bool librdr3::ExportYnv(std::string filePath, std::shared_ptr<CNavmeshData> data) {
    try {
        bStream::CFileStream ynvStream(filePath, bStream::Little, bStream::Out);
        data->Serialize(&ynvStream);
    }
    catch (std::exception e) {
        std::cout << "Failed to save navmesh to YNV: " << e.what() << "\n";
        return false;
    }

    return true;
}
