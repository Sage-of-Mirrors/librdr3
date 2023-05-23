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

    UGeometry() { }

    std::vector<std::array<float, 3>> GetVertexPositionArray() {
        std::vector<std::array<float, 3>> t;

        for (int i = 0; i < Vertices.size(); i++) {
            std::array<float, 3> b;
            b[0] = Vertices[i]->Position[0].x;
            b[1] = Vertices[i]->Position[0].y;
            b[2] = Vertices[i]->Position[0].z;

            t.push_back(b);
        }

        return t;
    }

    std::vector<std::array<uint32_t, 3>> GetIndexArray() {
        std::vector<std::array<uint32_t, 3>> t;

        for (int i = 2; i < Indices.size(); i += 3) {
            std::array<uint32_t, 3> b;
            b[0] = Indices[i - 2];
            b[1] = Indices[i - 1];
            b[2] = Indices[i];

            t.push_back(b);
        }

        return t;
    }

    virtual ~UGeometry() {
        for (int i = 0; i < Vertices.size(); i++) {
            delete Vertices[i];
            Vertices[i] = nullptr;
        }

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

    UDrawable() : FileName(""), Skeleton(nullptr) { }

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
