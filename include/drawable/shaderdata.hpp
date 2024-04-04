#pragma once

#include "types.h"
#include "util/math.hpp"

#include <vector>
#include <string>
#include <memory>

struct CShader;
struct CShaderUniform;

enum class EParameterType : uint8_t {
    TEXTURE,
    UNKNOWN,
    SAMPLER,
    PARAMETER
};

class CTextureData {
public:
    CTextureData();
    virtual ~CTextureData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);
};

class CShaderUniformData {
    uint32_t mNameHash;

    EParameterType mType;
    uint8_t mIndex;
    uint16_t mBufferIndex;
    uint16_t mUniformOffset;
    uint16_t mUniformSize;

    float mData[64] {};

    std::string mName;

public:
    CShaderUniformData();
    virtual ~CShaderUniformData();

    void Deserialize(bStream::CStream* stream, uint64_t parametersPtr);
    void Serialize(bStream::CStream* stream);

    std::shared_ptr<CShaderUniform> GetShaderUniform();
};

class CShaderData {
    uint32_t mNameHash;
    uint32_t mType;

    shared_vector<CShaderUniformData> mUniforms;
    shared_vector<CTextureData> mTextures;

    uint64_t m0018;
    
    uint64_t m0028;
    uint64_t m0030;

    uint8_t m0038;
    uint8_t mDrawBucket;

    uint32_t mRenderBucketMask;

    std::string mName;

public:
    CShaderData();
    virtual ~CShaderData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    std::shared_ptr<CShader> GetShader();
};

class CShaderContainer {
    uint64_t mVTable;
    uint64_t mTextureDictionaryPtr;

    shared_vector<CShaderData> mShaders;
    uint32_t m001C;

    uint64_t m0020;
    uint16_t m0028;
    uint16_t m002A;
    uint32_t m002C;

    uint64_t m0030;
    uint64_t m0038;

public:
    CShaderContainer();
    virtual ~CShaderContainer();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    shared_vector<CShader> GetShaders();
};
