#pragma once

#include <string>

struct UDrawable;
struct UDrawableDictionary;
namespace UNavmesh {
    class  UNavmeshData;
}
struct UFragment;

UDrawable*              ImportYdr(std::string filePath);
UDrawableDictionary*    ImportYdd(std::string filePath);
UFragment*              ImportYft(std::string filePath);
UNavmesh::UNavmeshData* ImportYnv(std::string filePath);

bool ExportYdr(std::string filePath, UDrawable* data);
