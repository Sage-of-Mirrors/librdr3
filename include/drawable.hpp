#pragma once

#include "math.hpp"
#include "vertexdata.hpp"

#include <array>
#include <vector>
#include <string>

struct UGeometry {
    std::vector<UVertex> Vertices;
    std::vector<uint32_t> Indices;

    UGeometry() { }

    virtual ~UGeometry() {
        Vertices.clear();
        Indices.clear();
    }
};

struct UModel {
    std::vector<UGeometry*> Geometries;

    UModel() { }

    virtual ~UModel() {
        for (int i = 0; i < Geometries.size(); i++) {
            delete Geometries[i];
            Geometries[i] = nullptr;
        }
    }
};

struct ULod {
    std::vector<UModel*> Models;

    float LodDistance;
    uint32_t LodFlags;

    ULod() : LodDistance(0.0f), LodFlags(0) { }

    virtual ~ULod() {
        for (int i = 0; i < Models.size(); i++) {
            delete Models[i];
            Models[i] = nullptr;
        }
    }
};

struct UDrawable {
    std::string FileName;
    std::array<ULod*, 4> Lods = { nullptr, nullptr, nullptr, nullptr };

    UDrawable() : FileName("") { }

    virtual ~UDrawable() {
        for (int i = 0; i < 4; i++) {
            if (Lods[i] != nullptr) {
                delete Lods[i];
                Lods[i] = nullptr;
            }
        }
    }
};
