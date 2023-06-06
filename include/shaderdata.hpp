#pragma once

#include "types.h"
#include "math.hpp"

#include <vector>
#include <string>

struct UShader;
struct UShaderUniform;

enum class EParameterType : uint8_t {
    PRM_TEXTURE,
    PRM_UNKNOWN,
    PRM_SAMPLER,
    PRM_PARAMETER
};

class UTextureData {
public:
    UTextureData();
    virtual ~UTextureData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);
};

class UShaderUniformData {
    uint32_t mNameHash;

    EParameterType mType;
    uint8_t mIndex;
    uint16_t mBufferIndex;
    uint16_t mUniformOffset;
    uint16_t mUniformSize;

    float mData[64] {};

    std::string mName;

public:
    UShaderUniformData();
    virtual ~UShaderUniformData();

    void Deserialize(bStream::CStream* stream, uint64_t parametersPtr);
    void Serialize(bStream::CStream* stream);

    UShaderUniform* GetShaderUniform();
};

class UShaderData {
    uint32_t mNameHash;
    uint32_t mType;

    std::vector<UShaderUniformData*> mUniforms;
    std::vector<UTextureData*> mTextures;

    uint64_t m0018;
    
    uint64_t m0028;
    uint64_t m0030;

    uint8_t m0038;
    uint8_t mDrawBucket;

    uint32_t mRenderBucketMask;

    std::string mName;

public:
    UShaderData();
    virtual ~UShaderData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    UShader* GetShader();
};

class UShaderContainer {
    uint64_t mVTable;
    uint64_t mTextureDictionaryPtr;

    std::vector<UShaderData*> mShaders;
    uint32_t m001C;

    uint64_t m0020;
    uint16_t m0028;
    uint16_t m002A;
    uint32_t m002C;

    uint64_t m0030;
    uint64_t m0038;

public:
    UShaderContainer();
    virtual ~UShaderContainer();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    std::vector<UShader*> GetShaders();
};
