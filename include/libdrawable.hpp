#pragma once

#include <string>

struct UDrawable;
struct UDrawableDictionary;
struct UNavmesh;
struct UFragment;

UDrawable* ImportYdr(std::string filePath);
UDrawableDictionary* ImportYdd(std::string filePath);
UFragment* ImportYft(std::string filePath);
UNavmesh* ImportYnv(std::string filePath);

bool ExportYdr(std::string filePath, UDrawable* data);
