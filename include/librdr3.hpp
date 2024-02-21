#pragma once

#include "ynv/navmeshdata.hpp"

#include <string>
#include <memory>

struct UDrawable;
struct UDrawableDictionary;

struct UFragment;

namespace librdr3 {
    using UNavmeshShared = std::shared_ptr<UNavmesh::UNavmeshData>;

    UDrawable*           ImportYdr(std::string filePath);
    UDrawableDictionary* ImportYdd(std::string filePath);
    UFragment*           ImportYft(std::string filePath);
    UNavmeshShared       ImportYnv(std::string filePath);

    bool ExportYdr(std::string filePath, UDrawable* data);
    bool ExportYnv(std::string filePath, UNavmeshShared data);

}
