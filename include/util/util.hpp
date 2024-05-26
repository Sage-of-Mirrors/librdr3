#pragma once

#include "types.h"
#include <string>

uint32_t JenkinsHashString(std::string str);

constexpr uint32_t operator""_jenkins(char const * const data, size_t const size) {
    uint32_t hash = 0;

    for (size_t i = 0; i < size; i++) {
        hash += (uint8_t)data[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
}
