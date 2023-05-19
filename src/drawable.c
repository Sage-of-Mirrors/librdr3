#include <stdlib.h>

#include "drawable.h"
#include "stream.h"
#include "blockmap.h"
#include "shadergroup.h"
#include "loddata.h"

void DeserializeDrawable(UStream* stream, UDrawable* dst) {
    if (stream == NULL || dst == NULL) {
        return;
    }

    size_t streamPos = 0;
    dst->mVTable = UStreamReadU64(stream);
    
    // Read blockmap
    uint64_t blockMapPtr = UStreamReadU64(stream) - 0x50000000;
    dst->mBlockMap = (UBlockMap*)malloc(sizeof(UBlockMap));

    streamPos = stream->position;
    UStreamSeek(stream, blockMapPtr, 0);
    DeserializeBlockMap(stream, dst->mBlockMap);
    stream->position = streamPos;

    // Read shader data

    uint64_t shaderGroupPtr = UStreamReadU64(stream) - 0x50000000;
    dst->mShaderGroup = (UShaderGroup*)malloc(sizeof(UShaderGroup));

    streamPos = stream->position;
    UStreamSeek(stream, shaderGroupPtr, 0);
    DeserializeShaderGroup(stream, dst->mShaderGroup);
    stream->position = streamPos;

    // Read skeleton data, if present
    uint64_t skeletonDataPtr = UStreamReadU64(stream);
    if (skeletonDataPtr != 0) {
        /*
        dst->mSkeletonData = (USkeletonData*)malloc(sizeof(USkeletonData));

        streamPos = stream->position;
        UStreamSeek(stream, skeletonDataPtr - 0x50000000, 0);
        DeserializeSkeletonData(stream, dst->mSkeletonData);
        stream->position = streamPos;
        */
    }
    else {
        dst->mSkeletonData = NULL;
    }

    // Read bounding sphere
    dst->mBoundingSphere.x = UStreamReadFloat(stream);

    dst->mBoundingSphere.z = UStreamReadFloat(stream); // Up-axis conversion - swap Y and Z
    dst->mBoundingSphere.y = UStreamReadFloat(stream);

    dst->mBoundingSphere.w = UStreamReadFloat(stream); // Radius of the sphere

    // Read bounding box minimum
    dst->mBoundingBoxMin.x = UStreamReadFloat(stream);

    dst->mBoundingBoxMin.z = UStreamReadFloat(stream); // Up-axis conversion - swap Y and Z
    dst->mBoundingBoxMin.y = UStreamReadFloat(stream);

    dst->mBoundingBoxMin.w = UStreamReadFloat(stream); // Useless

    // Read bounding box maximum
    dst->mBoundingBoxMax.x = UStreamReadFloat(stream);

    dst->mBoundingBoxMax.z = UStreamReadFloat(stream); // Up-axis conversion - swap Y and Z
    dst->mBoundingBoxMax.y = UStreamReadFloat(stream);

    dst->mBoundingBoxMax.w = UStreamReadFloat(stream); // Useless

    // Read LOD data
    for (int i = 0; i < LOD_MAX; i++) {
        uint64_t lodDataPtr = UStreamReadU64(stream);

        if (lodDataPtr == 0) {
            dst->mLodData[i] = NULL;
            continue;
        }
        
        dst->mLodData[i] = (ULodData*)malloc(sizeof(ULodData));

        streamPos = stream->position;
        UStreamSeek(stream, lodDataPtr & 0x0FFFFFFF, 0);
        DeserializeLodData(stream, dst->mLodData[i]);
        stream->position = streamPos;
    }

    // Read LOD distances
    for (int i = 0; i < LOD_MAX; i++) {
        dst->mLodDistances[i] = UStreamReadFloat(stream);
    }

    // Read LOD flags
    for (int i = 0; i < LOD_MAX; i++) {
        dst->mLodFlags[i] = UStreamReadU32(stream);
    }

    // Read joint limit data, if present
    uint64_t jointLimitDataPtr = UStreamReadU64(stream);
    if (jointLimitDataPtr != 0) {
        /*
        dst->mJointLimitData = (UJointLimitData*)malloc(sizeof(UJointLimitData));

        streamPos = stream->position;
        UStreamSeek(stream, jointLimitDataPtr - 0x50000000, 0);
        DeserializeJointLimitData(stream, dst->mJointLimitData);
        stream->position = streamPos;
        */
    }
    else {
        dst->mJointLimitData = NULL;
    }
    dst->mPadding1 = UStreamReadU64(stream);

    // Read expression count
    dst->mExpressionCount = UStreamReadU16(stream);
    dst->mPadding2 = UStreamReadU16(stream);

    // Read expressions
    dst->mExpressions = UStreamReadU32(stream);

    // Read name
    char* name;
    uint64_t namePtr = UStreamReadU64(stream) - 0x50000000;

    streamPos = stream->position;
    UStreamSeek(stream, namePtr, 0);
    name = UStreamReadStr(stream, 256);
    stream->position = streamPos;

    dst->m00B0 = UStreamReadU64(stream);

    dst->mBoundPointer = UStreamReadU64(stream);
    dst->mSamplers = UStreamReadU64(stream);
    dst->mPadding3 = UStreamReadU64(stream);
}

void SerializeDrawable(UStream* stream, UDrawable* src) {

}
