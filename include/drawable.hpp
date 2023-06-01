#pragma once

#include "math.hpp"
#include "vertexdata.hpp"
#include "skeleton.hpp"

#include <iostream>
#include <array>
#include <vector>
#include <string>

struct UGeometry {
    std::vector<UVertex*> Vertices;
    std::vector<uint32_t> Indices;

    uint32_t PositionsCount = 0;
    uint32_t NormalsCount = 0;
    uint32_t TangentsCount = 0;
    uint32_t BinormalsCount = 0;
    uint32_t BlendIndicesCount = 0;
    uint32_t BlendWeightsCount = 0;
    uint32_t ColorsCount = 0;
    uint32_t TexCoordsCount = 0;

    UGeometry() { }

    virtual ~UGeometry() {
        for (int i = 0; i < Vertices.size(); i++) {
            delete Vertices[i];
            Vertices[i] = nullptr;
        }

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
    USkeleton* Skeleton;

    uint32_t DictionaryHash;

    UDrawable() : FileName(""), Skeleton(nullptr), DictionaryHash(0) { }

    virtual ~UDrawable() {
        for (int i = 0; i < 4; i++) {
            if (Lods[i] != nullptr) {
                delete Lods[i];
                Lods[i] = nullptr;
            }
        }

        delete Skeleton;
        Skeleton = nullptr;
    }
};
