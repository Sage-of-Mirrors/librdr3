#ifndef LIBDRAWABLE_SHADERGROUP_H
#define LIBDRAWABLE_SHADERGROUP_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct UStream UStream;

    typedef struct UShader {
        uint32_t mNameHash; // 0x00
        char mTag[4];       // 0x04

        uint64_t mParameters;  // 0x08
        uint64_t mTextureRefs; // 0x10
        uint64_t m0018;        // 0x18

        uint64_t mParameterData; // 0x20
        uint64_t m0028;          // 0x28
        uint64_t m0030;          // 0x30
        uint8_t m0038;           // 0x38

        uint8_t mDrawBucket;         // 0x39
        uint16_t mParameterDataSize; // 0x3A
        uint32_t mRenderBucketMask;  // 0x3C
    } UShader;

    void DeserializeShader(UStream* stream, UShader* dst);
    void SerializeShader(UStream* stream, UShader* src);

    typedef struct UShaderGroup {
        uint64_t mVTable;            // 0x00
        uint64_t mTextureDictionary; // 0x08

        UShader* mShaders;        // 0x10
        uint16_t mShaderCount;    // 0x18
        uint16_t mShaderCapacity; // 0x1A
        uint32_t m001C;           // 0x1C

        uint64_t mUnkArray;         // 0x20
        uint16_t mUnkArrayCount;    // 0x28
        uint16_t mUnkArrayCapacity; // 0x2A
        uint32_t m002C;             // 0x2C

        uint32_t m0030; // 0x30
        uint32_t m0034; // 0x34
        uint32_t m0038; // 0x38
        uint32_t m003C; // 0x3C
    } UShaderGroup;

    void DeserializeShaderGroup(UStream* stream, UShaderGroup* dst);
    void SerializeShaderGroup(UStream* stream, UShaderGroup* src);

#ifdef __cplusplus
}
#endif

#endif