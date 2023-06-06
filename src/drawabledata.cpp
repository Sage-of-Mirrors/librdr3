#include "drawabledata.hpp"
#include "loddata.hpp"
#include "skeletondata.hpp"
#include "bstream.h"
#include "drawable.hpp"


/* UDrawable */

UDrawableData::UDrawableData() : mBlockMap(nullptr), mSkeletonData(nullptr) {

}

UDrawableData::~UDrawableData() {

}

void UDrawableData::Deserialize(bStream::CStream* stream) {
    if (stream == nullptr) {
        return;
    }

    size_t streamPos = 0;
    mVTable = stream->readUInt64();

    // Read blockmap
    uint64_t blockMapPtr = stream->readUInt64() & 0x0FFFFFFF;
    if (blockMapPtr != 0) {
        streamPos = stream->tell();
        stream->seek(blockMapPtr);
        //mBlockMap.Deserialize(stream);

        stream->seek(streamPos);
    }

    // Read shader data

    uint64_t shaderGroupPtr = stream->readUInt64() & 0x0FFFFFFF;
    streamPos = stream->tell();
    stream->seek(shaderGroupPtr);
    mShaderContainer.Deserialize(stream);

    stream->seek(streamPos);

    // Read skeleton data, if present
    uint64_t skeletonDataPtr = stream->readUInt64() & 0x0FFFFFFF;
    if (skeletonDataPtr != 0) {
        mSkeletonData = new USkeletonData();
        streamPos = stream->tell();

        stream->seek(skeletonDataPtr);
        mSkeletonData->Deserialize(stream);

        stream->seek(streamPos);
    }

    // Read bounding sphere
    mBoundingSphere.x = stream->readFloat();
    mBoundingSphere.z = stream->readFloat(); // Up-axis conversion - swap Y and Z
    mBoundingSphere.y = stream->readFloat();
    mBoundingSphere.w = stream->readFloat(); // Radius of the sphere

    // Read bounding box minimum
    mBoundingBoxMin.x = stream->readFloat();
    mBoundingBoxMin.z = stream->readFloat(); // Up-axis conversion - swap Y and Z
    mBoundingBoxMin.y = stream->readFloat();
    mBoundingBoxMin.w = stream->readFloat(); // Useless

    // Read bounding box maximum
    mBoundingBoxMax.x = stream->readFloat();
    mBoundingBoxMax.z = stream->readFloat(); // Up-axis conversion - swap Y and Z
    mBoundingBoxMax.y = stream->readFloat();
    mBoundingBoxMax.w = stream->readFloat(); // Useless

    // Read LOD data
    for (int i = 0; i < LOD_MAX; i++) {
        uint64_t lodDataPtr = stream->readUInt64() & 0x0FFFFFFF;

        if (lodDataPtr == 0) {
            mLodData[i] = nullptr;
            continue;
        }

        mLodData[i] = new ULodData();

        streamPos = stream->tell();
        stream->seek(lodDataPtr);
        mLodData[i]->Deserialize(stream);

        stream->seek(streamPos);
    }

    // Read LOD distances
    for (int i = 0; i < LOD_MAX; i++) {
        mLodDistances[i] = stream->readFloat();
    }

    // Read LOD flags
    for (int i = 0; i < LOD_MAX; i++) {
        mLodFlags[i] = stream->readUInt32();
    }

    // Read joint limit data, if present
    uint64_t jointLimitDataPtr = stream->readUInt64() & 0x0FFFFFFF;
    if (jointLimitDataPtr != 0) {
        /*
        mJointLimitData = new UJointLimitData();

        streamPos = stream->tell();
        stream->seek(jointLimitDataPtr);
        mJointLimitData->Deserialize(stream);

        stream->seek(streamPos);
        */
    }

    mPadding1 = stream->readUInt64();

    // Read expression count
    mExpressionCount = stream->readUInt16();
    mPadding2 = stream->readUInt16();

    // Read expressions
    mExpressions = stream->readUInt32();

    // Read name
    uint64_t namePtr = stream->readUInt64() & 0x0FFFFFFF;

    streamPos = stream->tell();
    stream->seek(namePtr);
    mName = stream->readString(256);

    stream->seek(streamPos);

    m00B0 = stream->readUInt64();
    mBoundPointer = stream->readUInt64();
    mSamplers = stream->readUInt64();
    mPadding3 = stream->readUInt64();
}

void UDrawableData::Serialize(bStream::CStream* stream) {

}

UDrawable* UDrawableData::GetDrawable() {
    UDrawable* drawable = new UDrawable();
    drawable->FileName = mName.data();

    std::vector<UShader*> shaders = mShaderContainer.GetShaders();

    for (int i = 0; i < LOD_MAX; i++) {
        if (mLodData[i] == nullptr) {
            continue;
        }

        drawable->Lods[i] = mLodData[i]->GetLod();
        drawable->Lods[i]->LodDistance = mLodDistances[i];
        drawable->Lods[i]->LodFlags = mLodFlags[i];

        for (UModel* m : drawable->Lods[i]->Models) {
            for (UGeometry* g : m->Geometries) {
                g->Shader = new UShader(*shaders[g->ShaderIndex]);
            }
        }
    }

    if (mSkeletonData != nullptr) {
        drawable->Skeleton = mSkeletonData->GetSkeleton();
    }

    for (int i = 0; i < shaders.size(); i++) {
        delete shaders[i];
    }

    return drawable;
}