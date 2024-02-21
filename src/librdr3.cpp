#include "librdr3.hpp"
#include "bstream.h"

#include "drawable.hpp"
#include "drawabledata.hpp"

#include "drawabledictionary.hpp"
#include "drawabledictionarydata.hpp"

UDrawable* librdr3::ImportYdr(std::string filePath) {
    bStream::CFileStream stream(filePath);
    if (stream.peekUInt32(0) != 0) {
        return nullptr;
    }

    UDrawableData data;
    data.Deserialize(&stream);

    return data.GetDrawable();
}

UDrawableDictionary* librdr3::ImportYdd(std::string filePath) {
    bStream::CFileStream stream(filePath);
    if (stream.peekUInt32(0) != 0) {
        return nullptr;
    }

    UDrawableDictionaryData data;
    data.Deserialize(&stream);

    return data.GetDrawableDictionary();
}

UFragment* librdr3::ImportYft(std::string filePath) {
    bStream::CFileStream stream(filePath);
    if (stream.peekUInt32(0) != 0) {
        return nullptr;
    }
}

std::shared_ptr<UNavmesh::UNavmeshData> librdr3::ImportYnv(std::string filePath) {
    bStream::CFileStream stream(filePath);
    if (stream.peekUInt32(0) != 0) {
        return nullptr;
    }

    std::shared_ptr<UNavmesh::UNavmeshData> data = std::make_shared<UNavmesh::UNavmeshData>();
    data->Deserialize(&stream);

    return data;
}

bool librdr3::ExportYdr(std::string filePath, UDrawable* data) {
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

bool librdr3::ExportYnv(std::string filePath, UNavmeshShared data) {
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
