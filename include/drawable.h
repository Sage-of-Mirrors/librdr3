#ifndef LIBDRAWABLE_DRAWABLE_H
#define LIBDRAWABLE_DRAWABLE_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct UStream UStream;

    typedef struct UBlockMap UBlockMap;
    typedef struct UShaderGroup UShaderGroup;
    typedef struct ULodData ULodData;
    typedef struct UJointLimitData UJointLimitData;

    typedef struct {
      uint64_t mVTable;            // 0x00

      UBlockMap* mBlockMap;        // 0x08
      UShaderGroup* mShaderGroup;  // 0x10
      uint64_t mSkeletonData;      // 0x18
  
      UVector4 mBoundingSphere; // 0x20, w is radius
      UVector4 mBoundingBoxMin; // 0x30, w has no meaning
      UVector4 mBoundingBoxMax; // 0x40, w has no meaning
  
      ULodData* mLodData[LOD_MAX];   // 0x50, 0x58, 0x60, 0x68
      float mLodDistances[LOD_MAX];  // 0x70, 0x74, 0x78, 0x7C
      uint32_t mLodFlags[LOD_MAX];   // 0x80, 0x84, 0x88, 0x8C
  
      UJointLimitData* mJointLimitData; // 0x90
      uint64_t mPadding1;   // 0x98
  
      uint16_t mExpressionCount;  // 0xA0
      uint16_t mPadding2;         // 0xA2
      uint32_t mExpressions;      // 0xA4
  
      uint64_t mName;         // 0xA8
      uint64_t m00B0;         // 0xB0
      uint64_t mBoundPointer; // 0xB8
      uint64_t mSamplers;     // 0xC0
      uint64_t mPadding3;     // 0xC8
    } UDrawable;

    void DeserializeDrawable(UStream* stream, UDrawable* dst);
    void SerializeDrawable(UStream* stream, UDrawable* src);

#ifdef __cplusplus
}
#endif

#endif