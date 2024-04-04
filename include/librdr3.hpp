#pragma once

#include "navmesh/navmeshdata.hpp"

#include <string>
#include <memory>

struct CDrawable;
struct CDrawableDictionary;
struct CFragment;

namespace librdr3 {
    std::shared_ptr<CDrawable>           ImportYdr(std::string filePath);
    std::shared_ptr<CDrawableDictionary> ImportYdd(std::string filePath);
    std::shared_ptr<CFragment>           ImportYft(std::string filePath);
    std::shared_ptr<CNavmeshData>        ImportYnv(std::string filePath);

    bool ExportYdr(std::string filePath, std::shared_ptr<CDrawable> data);
    bool ExportYnv(std::string filePath, std::shared_ptr<CNavmeshData> data);

}
