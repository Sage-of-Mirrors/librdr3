#pragma once

#include "math.hpp"
#include "vertexdata.hpp"
#include "skeleton.hpp"

#include <iostream>
#include <array>
#include <vector>
#include <string>

struct UShaderUniform {
    std::string Name = "";
    uint32_t Hash = 0;
    std::vector<float> Data;

    UShaderUniform() { }

    UShaderUniform(UShaderUniform& src) {
        Name = src.Name.data();
        Hash = src.Hash;

        for (int i = 0; i < src.Data.size(); i++) {
            Data.push_back(src.Data[i]);
        }
    }
};

struct UShader {
    std::string Name = "";
    uint32_t Hash = 0;
    std::vector<UShaderUniform*> Uniforms;

    UShader() { }

    UShader(UShader& src) {
        Name = src.Name.data();
        Hash = src.Hash;

        for (UShaderUniform* u : src.Uniforms) {
            Uniforms.push_back(new UShaderUniform(*u));
        }
    }

    virtual ~UShader() {
        for (int i = 0; i < Uniforms.size(); i++) {
            delete Uniforms[i];
            Uniforms[i] = nullptr;
        }

        Uniforms.clear();
    }
};

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

    uint16_t ShaderIndex;
    UShader* Shader = nullptr;

    virtual ~UGeometry() {
        for (int i = 0; i < Vertices.size(); i++) {
            delete Vertices[i];
            Vertices[i] = nullptr;
        }

        if (Shader) {
            try {
                delete Shader;
            }
            catch (std::exception e) {

            }

            Shader = nullptr;
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

    float LodDistance = 0.0f;
    uint32_t LodFlags = 0;

    virtual ~ULod() {
        for (int i = 0; i < Models.size(); i++) {
            delete Models[i];
            Models[i] = nullptr;
        }
    }
};

struct UDrawable {
    std::string FileName = "";
    std::array<ULod*, 4> Lods = { nullptr, nullptr, nullptr, nullptr };
    USkeleton* Skeleton = nullptr;

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
