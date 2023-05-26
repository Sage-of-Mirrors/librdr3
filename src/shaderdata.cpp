#include "shaderdata.hpp"
#include "bstream.h"

#include "util.hpp"
#include "shadernames.hpp"
#include "shaderparamnames.hpp"


/* UTextureData */

UTextureData::UTextureData() {

}

UTextureData::~UTextureData() {

}

void UTextureData::Deserialize(bStream::CStream* stream) {

}

void UTextureData::Serialize(bStream::CStream* stream) {

}


/* UShaderParameter */

UShaderParameter::UShaderParameter() : mName("") {

}

UShaderParameter::~UShaderParameter() {

}

void UShaderParameter::Deserialize(bStream::CStream* stream, uint64_t parametersPtr) {
    mNameHash = stream->readUInt32();
    uint32_t packedField = stream->readUInt32();

    mType  =  (EParameterType)(packedField & 0x00000003);
    mIndex = (packedField & 0x000000FC) >> 0x02;

    mBufferIndex     = (packedField & 0x000000C0) >> 0x02;
    mParameterOffset = (packedField & 0x000FFF00) >> 0x08;
    mParameterSize   = (packedField & 0xFFF00000) >> 0x14;

    mParameterSize /= sizeof(float);
    assert(mParameterSize <= 4);

    if (mType == EParameterType::PRM_PARAMETER) {
        // Calculate offset for parameter data

        uint64_t bufferOffset = mBufferIndex * sizeof(uint64_t);
        uint64_t dataPtr = stream->peekUInt64(parametersPtr + bufferOffset) & 0x0FFFFFFF;
        dataPtr += mParameterOffset;

        // Read parameter data
        size_t streamPos = stream->tell();
        stream->seek(dataPtr);

        for (int i = 0; i < mParameterSize; i++) {
            mData[i] = stream->readFloat();
        }

        stream->seek(streamPos);
    }

    // Try to find parameter name
    for (int i = 0; i < LEN_SHADER_PARAM_NAMES; i++) {
        uint32_t hash = HashString(SHADER_PARAM_NAMES[i]);

        if (mNameHash == hash) {
            mName = SHADER_PARAM_NAMES[i];
            break;
        }
    }

    if (mName == "") {
        mName = std::to_string(mNameHash);
    }
}

void UShaderParameter::Serialize(bStream::CStream* stream) {

}


/* UShader */

UShader::UShader() : mName("") {

}

UShader::~UShader() {
    for (int i = 0; i < mParameters.size(); i++) {
        delete mParameters[i];
        mParameters[i] = nullptr;
    }

    mParameters.clear();

    for (int i = 0; i < mTextures.size(); i++) {
        delete mTextures[i];
        mTextures[i] = nullptr;
    }

    mTextures.clear();
}

void UShader::Deserialize(bStream::CStream* stream) {
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
        UShaderParameter* newParam = new UShaderParameter();
        newParam->Deserialize(stream, parametersPtr);

        mParameters.push_back(newParam);
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

        UTextureData* tex = new UTextureData();
        tex->Deserialize(stream);
        mTextures.push_back(tex);

        stream->seek(streamPos);
    }

    // Try to find shader name

    for (int i = 0; i < LEN_SHADER_NAMES; i++) {
        uint32_t hash = HashString(SHADER_NAMES[i]);

        if (mNameHash == hash) {
            mName = SHADER_NAMES[i];
            break;
        }
    }

    if (mName == "") {
        mName = std::to_string(mNameHash);
    }
}

void UShader::Serialize(bStream::CStream* stream) {

}


/* UShaderGroup */

UShaderGroup::UShaderGroup() {

}

UShaderGroup::~UShaderGroup() {
    for (int i = 0; i < mShaders.size(); i++) {
        delete mShaders[i];
        mShaders[i] = nullptr;
    }

    mShaders.clear();
}

void UShaderGroup::Deserialize(bStream::CStream* stream) {
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

        UShader* newShader = new UShader();
        newShader->Deserialize(stream);
        mShaders.push_back(newShader);

        stream->seek(streamPos);
    }
}

void UShaderGroup::Serialize(bStream::CStream* stream) {

}

void UShaderGroup::GetShaders() {

}
