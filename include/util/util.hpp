#pragma once

#include "types.h"
#include <string>

uint32_t HashString(std::string str) {
    uint32_t hash = 0;

    for (int i = 0; i < str.length(); i++) {
        hash += (uint8_t)str[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
}
