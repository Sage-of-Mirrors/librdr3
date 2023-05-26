#pragma once

#include "types.h"
#include "math.hpp"

#include <vector>
#include <string>


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

class UShaderParameter {
    uint32_t mNameHash;

    EParameterType mType;
    uint8_t mIndex;
    uint16_t mBufferIndex;
    uint16_t mParameterOffset;
    uint16_t mParameterSize;

    float mData[4] { 0.0f, 0.0f, 0.0f, 0.0f };

    std::string mName;

public:
    UShaderParameter();
    virtual ~UShaderParameter();

    void Deserialize(bStream::CStream* stream, uint64_t parametersPtr);
    void Serialize(bStream::CStream* stream);
};

class UShader {
    uint32_t mNameHash;
    uint32_t mType;

    std::vector<UShaderParameter*> mParameters;
    std::vector<UTextureData*> mTextures;

    uint64_t m0018;
    
    uint64_t m0028;
    uint64_t m0030;

    uint8_t m0038;
    uint8_t mDrawBucket;

    uint32_t mRenderBucketMask;

    std::string mName;

public:
    UShader();
    virtual ~UShader();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);
};

class UShaderGroup {
    uint64_t mVTable;
    uint64_t mTextureDictionaryPtr;

    std::vector<UShader*> mShaders;
    uint32_t m001C;

    uint64_t m0020;
    uint16_t m0028;
    uint16_t m002A;
    uint32_t m002C;

    uint64_t m0030;
    uint64_t m0038;

public:
    UShaderGroup();
    virtual ~UShaderGroup();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    void GetShaders();
};
