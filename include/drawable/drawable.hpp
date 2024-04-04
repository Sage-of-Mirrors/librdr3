#pragma once

#include "types.h"
#include "vertexdata.hpp"
#include "skeleton.hpp"
#include "util/math.hpp"

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <memory>

struct CShaderUniform {
    std::string Name = "";
    uint32_t Hash = 0;
    std::vector<float> Data;

    CShaderUniform() { }

    CShaderUniform(CShaderUniform& src) {
        Name = src.Name.data();
        Hash = src.Hash;

        for (int i = 0; i < src.Data.size(); i++) {
            Data.push_back(src.Data[i]);
        }
    }
};

struct CShader {
    std::string Name = "";
    uint32_t Hash = 0;
    shared_vector<CShaderUniform> Uniforms;

    CShader() { }

    CShader(CShader& src) {
        Name = src.Name.data();
        Hash = src.Hash;

        for (std::shared_ptr<CShaderUniform> u : src.Uniforms) {
            Uniforms.push_back(std::shared_ptr<CShaderUniform>(new CShaderUniform(*u)));
        }
    }

    virtual ~CShader() {
        Uniforms.clear();
    }
};

struct CGeometry {
    shared_vector<CVertex> Vertices;
    std::vector<uint32_t> Indices;

    std::vector<EVertexFormat> VertexFormats;

    uint32_t PositionsCount = 0;
    uint32_t NormalsCount = 0;
    uint32_t TangentsCount = 0;
    uint32_t BinormalsCount = 0;
    uint32_t BlendIndicesCount = 0;
    uint32_t BlendWeightsCount = 0;
    uint32_t ColorsCount = 0;
    uint32_t TexCoordsCount = 0;

    uint16_t ShaderIndex;
    std::shared_ptr<CShader> Shader = nullptr;

    virtual ~CGeometry() {
        Vertices.clear();
        Indices.clear();
    }
};

struct CModel {
    shared_vector<CGeometry> Geometries;

    CModel() { }

    virtual ~CModel() {
        Geometries.clear();
    }
};

struct CLod {
    shared_vector<CModel> Models;

    float LodDistance = 0.0f;
    uint32_t LodFlags = 0;

    virtual ~CLod() {
        Models.clear();
    }
};

struct CDrawable {
    std::string FileName = "";
    std::array<std::shared_ptr<CLod>, 4> Lods = { nullptr, nullptr, nullptr, nullptr };
    std::shared_ptr<CSkeleton> Skeleton = nullptr;

    uint32_t DictionaryHash;

    CDrawable() : FileName(""), Skeleton(nullptr), DictionaryHash(0) { }

    virtual ~CDrawable() {

    }
};
