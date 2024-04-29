#include "drawable/shaderdata.hpp"
#include "drawable/shadernames.hpp"
#include "drawable/shaderparamnames.hpp"
#include "drawable/drawable.hpp"

#include "util/bstream.h"
#include "util/util.hpp"

#include <fstream>

/* CTextureData */

CTextureData::CTextureData() {

}

CTextureData::~CTextureData() {

}

void CTextureData::Deserialize(bStream::CStream* stream) {

}

void CTextureData::Serialize(bStream::CStream* stream) {

}


/* CShaderParameter */

CShaderUniformData::CShaderUniformData() : mName("") {

}

CShaderUniformData::~CShaderUniformData() {

}

void CShaderUniformData::Deserialize(bStream::CStream* stream, uint64_t parametersPtr) {
    mNameHash = stream->readUInt32();
    uint32_t packedField = stream->readUInt32();

    mType  =  (EParameterType)(packedField & 0x00000003);
    mIndex = (packedField & 0x000000FC) >> 0x02;

    mBufferIndex   = (packedField & 0x000000C0) >> 0x02;
    mUniformOffset = (packedField & 0x000FFF00) >> 0x08;
    mUniformSize   = (packedField & 0xFFF00000) >> 0x14;

    if (mBufferIndex >= 4) {
        return;
    }

    mUniformSize /= sizeof(float);
    assert(mUniformSize <= 64);

    if (mType == EParameterType::PARAMETER) {
        // Calculate offset for parameter data

        uint64_t bufferOffset = mBufferIndex * sizeof(uint64_t);
        uint64_t dataPtr = stream->peekUInt64(parametersPtr + bufferOffset) & 0x0FFFFFFF;
        dataPtr += mUniformOffset;

        // Read parameter data
        size_t streamPos = stream->tell();
        stream->seek(dataPtr);

        for (int i = 0; i < mUniformSize; i++) {
            mData[i] = stream->readFloat();
        }

        stream->seek(streamPos);
    }

    // Try to find parameter name
    for (int i = 0; i < LEN_SHADER_PARAM_NAMES; i++) {
        uint32_t hash = JenkinsHashString(SHADER_PARAM_NAMES[i]);

        if (mNameHash == hash) {
            mName = SHADER_PARAM_NAMES[i];
            break;
        }
    }

    if (mName == "") {
        mName = std::to_string(mNameHash);
    }
}

void CShaderUniformData::Serialize(bStream::CStream* stream) {

}

std::shared_ptr<CShaderUniform> CShaderUniformData::GetShaderUniform() {
    std::shared_ptr<CShaderUniform> uniform = std::make_shared<CShaderUniform>();

    uniform->Name = mName.data();
    uniform->Hash = mNameHash;

    for (int i = 0; i < mUniformSize; i++) {
        uniform->Data.push_back(mData[i]);
    }

    return uniform;
}


/* CShaderData */

CShaderData::CShaderData() : mName("") {

}

CShaderData::~CShaderData() {
    mUniforms.clear();
    mTextures.clear();
}

void CShaderData::Deserialize(bStream::CStream* stream) {
    mNameHash = stream->readUInt32();
    mType = stream->readUInt32();

    uint64_t parametersPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint64_t texturesPtr = stream->readUInt64() & 0x0FFFFFFF;

    m0018 = stream->readUInt64();

    uint64_t parametersDataPtr = stream->readUInt64() & 0x0FFFFFFF;

    m0028 = stream->readUInt64();
    m0030 = stream->readUInt64();

    m0038 = stream->readUInt8();
    mDrawBucket = stream->readUInt8();

    uint16_t parametersSize = stream->readUInt16();

    mRenderBucketMask = stream->readUInt32();

    // Read parameter data
    stream->seek(parametersDataPtr);

    uint8_t numBuffers = stream->readUInt8();
    uint8_t numTextures = stream->readUInt8();
    uint8_t numUnknown = stream->readUInt8();
    uint8_t numSamplers = stream->readUInt8();

    uint32_t numParameters = stream->readUInt32();

    for (int i = 0; i < numParameters; i++) {
        std::shared_ptr<CShaderUniformData> newParam = std::make_shared<CShaderUniformData>();
        newParam->Deserialize(stream, parametersPtr);

        mUniforms.push_back(newParam);
    }

    // Read texture data
    stream->seek(texturesPtr);
    size_t streamPos = 0;

    for (int i = 0; i < numTextures; i++) {
        uint64_t curTexturePtr = stream->readUInt64() & 0x0FFFFFFF;

        // There are specific texture slots, and sometimes a slot isn't filled.
        if (curTexturePtr == 0) {
            mTextures.push_back(nullptr);
            continue;
        }

        streamPos = stream->tell();
        stream->seek(curTexturePtr);

        std::shared_ptr<CTextureData> tex = std::make_shared<CTextureData>();
        tex->Deserialize(stream);
        mTextures.push_back(tex);

        stream->seek(streamPos);
    }

    // Try to find shader name

    for (int i = 0; i < LEN_SHADER_NAMES; i++) {
        uint32_t hash = JenkinsHashString(SHADER_NAMES[i]);

        if (mNameHash == hash) {
            mName = SHADER_NAMES[i];
            break;
        }
    }

    if (mName == "") {
        mName = std::to_string(mNameHash);
    }
}

void CShaderData::Serialize(bStream::CStream* stream) {

}

std::shared_ptr<CShader> CShaderData::GetShader() {
    std::shared_ptr<CShader> shader = std::make_shared<CShader>();

    shader->Name = mName.data();
    shader->Hash = mNameHash;

    for (std::shared_ptr<CShaderUniformData> d : mUniforms) {
        shader->Uniforms.push_back(d->GetShaderUniform());
    }

    return shader;
}


/* CShaderContainer */

CShaderContainer::CShaderContainer() {

}

CShaderContainer::~CShaderContainer() {
    mShaders.clear();
}

void CShaderContainer::Deserialize(bStream::CStream* stream) {
    mVTable = stream->readUInt64();
    mTextureDictionaryPtr = stream->readUInt64() & 0x0FFFFFFF;

    uint64_t shadersPtr = stream->readUInt64() & 0x0FFFFFFF;
    uint16_t shadersCount = stream->readUInt16();
    uint16_t shadersCapacity = stream->readUInt16();
    m001C = stream->readUInt32();

    m0020 = stream->readUInt64();
    m0028 = stream->readUInt16();
    m002A = stream->readUInt16();
    m002C = stream->readUInt32();

    m0030 = stream->readUInt64();
    m0038 = stream->readUInt64();

    // Read shaders

    stream->seek(shadersPtr);
    size_t streamPos = 0;

    for (int i = 0; i < shadersCount; i++) {
        uint64_t curShaderPtr = stream->readUInt64() & 0x0FFFFFFF;
        
        streamPos = stream->tell();
        stream->seek(curShaderPtr);

        std::shared_ptr<CShaderData> newShader = std::make_shared<CShaderData>();
        newShader->Deserialize(stream);
        mShaders.push_back(newShader);

        stream->seek(streamPos);
    }
}

void CShaderContainer::Serialize(bStream::CStream* stream) {

}

shared_vector<CShader> CShaderContainer::GetShaders() {
    shared_vector<CShader> shaders;

    for (std::shared_ptr<CShaderData> d : mShaders) {
        shaders.push_back(d->GetShader());
    }

    return shaders;
}
