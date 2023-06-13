#pragma once

#include <string>

struct UDrawable;
struct UDrawableDictionary;

UDrawable* ImportYdr(std::string filePath);
UDrawableDictionary* ImportYdd(std::string filePath);

bool ExportYdr(std::string filePath, UDrawable* data);
