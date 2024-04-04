#include "drawable/drawabledata.hpp"
#include "drawable/loddata.hpp"
#include "drawable/skeletondata.hpp"
#include "drawable/drawable.hpp"

#include "util/bstream.h"

#include <cmath>

/* CDrawable */

CDrawableData::CDrawableData() : mBlockMap(nullptr), mSkeletonData(nullptr) {
    mBoundingBoxMin.x = FLT_MAX;
    mBoundingBoxMin.y = FLT_MAX;
    mBoundingBoxMin.z = FLT_MAX;
    mBoundingBoxMin.w = 0.0f;

    mBoundingBoxMax.x = FLT_MIN;
    mBoundingBoxMax.y = FLT_MIN;
    mBoundingBoxMax.z = FLT_MIN;
    mBoundingBoxMax.w = 0.0f;
}

CDrawableData::~CDrawableData() {

}

void CDrawableData::Deserialize(bStream::CStream* stream) {
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
        mSkeletonData = std::make_shared<CSkeletonData>();
        streamPos = stream->tell();

        stream->seek(skeletonDataPtr);
        mSkeletonData->Deserialize(stream);

        stream->seek(streamPos);
    }

    // Read bounding sphere
    mBoundingSphere.x = stream->readFloat();
    mBoundingSphere.y = stream->readFloat();
    mBoundingSphere.z = stream->readFloat();
    mBoundingSphere.w = stream->readFloat(); // Radius of the sphere

    // Read bounding box minimum
    mBoundingBoxMin.x = stream->readFloat();
    mBoundingBoxMin.y = stream->readFloat();
    mBoundingBoxMin.z = stream->readFloat();
    mBoundingBoxMin.w = stream->readFloat(); // Useless

    // Read bounding box maximum
    mBoundingBoxMax.x = stream->readFloat();
    mBoundingBoxMax.y = stream->readFloat();
    mBoundingBoxMax.z = stream->readFloat();
    mBoundingBoxMax.w = stream->readFloat(); // Useless

    // Read LOD data
    for (int i = 0; i < LOD_MAX; i++) {
        uint64_t lodDataPtr = stream->readUInt64() & 0x0FFFFFFF;

        if (lodDataPtr == 0) {
            mLodData[i] = nullptr;
            continue;
        }

        mLodData[i] = std::make_shared<CLodData>();

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
        std::cout << "Joint limit data was not 0!" << std::endl;
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
    mCollisionPointer = stream->readUInt64();
    mSamplers = stream->readUInt64();
    mPadding3 = stream->readUInt64();
}

void CDrawableData::Serialize(bStream::CMemoryStream* stream) {
    // Empty header
    stream->fill(0xD0, 0);


    /* Write bounding volume data */
    stream->seek(0x20);

    // Bounding sphere
    stream->writeFloat(mBoundingSphere.x);
    stream->writeFloat(mBoundingSphere.y);
    stream->writeFloat(mBoundingSphere.z);
    stream->writeFloat(mBoundingSphere.w);

    // Bounding box min
    stream->writeFloat(mBoundingBoxMin.x);
    stream->writeFloat(mBoundingBoxMin.y);
    stream->writeFloat(mBoundingBoxMin.z);
    stream->writeFloat(mBoundingBoxMin.w);

    // Bounding box max
    stream->writeFloat(mBoundingBoxMax.x);
    stream->writeFloat(mBoundingBoxMax.y);
    stream->writeFloat(mBoundingBoxMax.z);
    stream->writeFloat(mBoundingBoxMax.w);


    /* Write LOD metadata */
    stream->seek(0x70);

    // Write LOD distances
    stream->writeFloat(mLodDistances[0]);
    stream->writeFloat(mLodDistances[1]);
    stream->writeFloat(mLodDistances[2]);
    stream->writeFloat(mLodDistances[3]);

    // Write LOD flags
    stream->writeUInt32(mLodFlags[0]);
    stream->writeUInt32(mLodFlags[1]);
    stream->writeUInt32(mLodFlags[2]);
    stream->writeUInt32(mLodFlags[3]);


    /* Write LOD data (models, geometry, vertex data) */
    for (int i = 0; i < LOD_MAX; i++) {
        if (mLodData[i] != nullptr) {
            //mLodData[i]->Serialize(stream, i);
        }
    }

    size_t streamEndOffset = stream->getSize();

    /* Write shader data */
    stream->seek(streamEndOffset);
    // TODO: serialize shader data

    // Write shader data offset
    stream->seek(0x10);
    stream->writeUInt64(streamEndOffset | 0x50000000);


    /* Write skeleton data */
    streamEndOffset = stream->getSize();
    stream->seek(streamEndOffset);
    // TODO: serialize skeleton data

    // Write skeleton data offset
    stream->seek(0x18);
    stream->writeUInt64(streamEndOffset | 0x50000000);


    /* Write filename */
    streamEndOffset = stream->getSize();
    stream->seek(streamEndOffset);
    stream->writeString(mName);

    // Write filename offset
    stream->seek(0xA8);
    stream->writeUInt64(streamEndOffset | 0x50000000);


    /* Write block map data */
    streamEndOffset = stream->getSize();
    stream->seek(streamEndOffset);
    // TODO: serialize block map data

    // Write block map offset
    stream->seek(0x08);
    stream->writeUInt64(streamEndOffset | 0x50000000);
}

std::shared_ptr<CDrawable> CDrawableData::GetDrawable() {
    std::shared_ptr<CDrawable> drawable = std::make_shared<CDrawable>();
    drawable->FileName = mName.data();

    shared_vector<CShader> shaders = mShaderContainer.GetShaders();

    for (int i = 0; i < LOD_MAX; i++) {
        if (mLodData[i] == nullptr) {
            continue;
        }

        drawable->Lods[i] = mLodData[i]->GetLod();
        drawable->Lods[i]->LodDistance = mLodDistances[i];
        drawable->Lods[i]->LodFlags = mLodFlags[i];

        for (std::shared_ptr<CModel> m : drawable->Lods[i]->Models) {
            for (std::shared_ptr<CGeometry> g : m->Geometries) {
                g->Shader = shaders[g->ShaderIndex];
            }
        }
    }

    if (mSkeletonData != nullptr) {
        drawable->Skeleton = mSkeletonData->GetSkeleton();
    }

    shaders.clear();
    return drawable;
}

void CDrawableData::SetDrawable(std::shared_ptr<CDrawable> drawable) {
    /* Set filename */
    mName = drawable->FileName;

    CalculateBoundingBox(drawable);
    CalculateBoundingSphere(drawable);

    /* Set LOD metadata */
    for (int i = 0; i < LOD_MAX; i++) {
        mLodDistances[i] = drawable->Lods[i] != nullptr ? drawable->Lods[i]->LodDistance : 9998.0f;
        mLodFlags[i] = drawable->Lods[i] != nullptr ? drawable->Lods[i]->LodFlags : 0;
    }
}

void CDrawableData::CalculateBoundingBox(std::shared_ptr<CDrawable> drawable) {
    for (int i = 0; i < LOD_MAX; i++) {
        if (drawable->Lods[i] == nullptr) {
            continue;
        }

        for (std::shared_ptr<CModel> model : drawable->Lods[i]->Models) {
            for (std::shared_ptr<CGeometry> geom : model->Geometries) {
                for (std::shared_ptr<CVertex> vert : geom->Vertices) {
                    Vector3 pos = vert->Position[0];

                    // Find minimum
                    if (pos.x < mBoundingBoxMin.x) {
                        mBoundingBoxMin.x = pos.x;
                    }
                    if (pos.y < mBoundingBoxMin.y) {
                        mBoundingBoxMin.y = pos.y;
                    }
                    if (pos.z < mBoundingBoxMin.z) {
                        mBoundingBoxMin.z = pos.z;
                    }

                    // Find maximum
                    if (pos.x > mBoundingBoxMax.x) {
                        mBoundingBoxMax.x = pos.x;
                    }
                    if (pos.y > mBoundingBoxMax.y) {
                        mBoundingBoxMax.y = pos.y;
                    }
                    if (pos.z > mBoundingBoxMax.z) {
                        mBoundingBoxMax.z = pos.z;
                    }
                }
            }
        }
    }
}

/*void UDrawableData::CalculateBoundingSphere(UDrawable* drawable) {
    float xDiff = mBoundingBoxMax.x - mBoundingBoxMin.x;
    float yDiff = mBoundingBoxMax.y - mBoundingBoxMin.y;
    float zDiff = mBoundingBoxMax.z - mBoundingBoxMin.z;

    float diameter = std::max(xDiff, std::max(yDiff, zDiff));
    float radius = diameter * 0.5f;
    float sqRadius = radius * radius;

    UVector3 center = ((mBoundingBoxMax + mBoundingBoxMin) * 0.5f).xyz();

    for (int i = 0; i < LOD_MAX; i++) {
        if (drawable->Lods[i] == nullptr) {
            continue;
        }

        for (UModel* model : drawable->Lods[i]->Models) {
            for (UGeometry* geom : model->Geometries) {
                for (UVertex* vert : geom->Vertices) {
                    UVector3 pos = vert->Position[0];

                    UVector3 direction = pos - center;
                    float sqDistance = direction.lengthSq();

                    if (sqDistance > sqRadius) {
                        float distance = std::sqrt(sqDistance);
                        float difference = distance - radius;

                        float newDiameter = (2.0f * radius) + difference;
                        radius = newDiameter * 0.5f;
                        sqRadius = radius * radius;

                        difference /= 2.0f;

                        center = center + ((direction / direction.length()) * difference);
                    }
                }
            }
        }
    }

    mBoundingSphere.x = center.x;
    mBoundingSphere.y = center.y;
    mBoundingSphere.z = center.z;
    mBoundingSphere.w = radius;
}*/

void CDrawableData::CalculateBoundingSphere(std::shared_ptr<CDrawable> drawable) {
    Vector3 center = { 0.f, 0.f, 0.f };
    float radius = 0.0f;
    float radiusSquared = 0.0f;

    for (int i = 0; i < LOD_MAX; i++) {
        if (drawable->Lods[i] == nullptr) {
            continue;
        }

        for (std::shared_ptr<CModel> model : drawable->Lods[i]->Models) {
            for (std::shared_ptr<CGeometry> geom : model->Geometries) {
                for (std::shared_ptr<CVertex> vert : geom->Vertices) {
                    Vector3 pos = vert->Position[0];

                    Vector3 posDiff = pos - center;
                    float distanceSquared = posDiff.lengthSq();

                    if (distanceSquared > radiusSquared) {
                        float distance = std::sqrtf(distanceSquared);
                        radius = (radius + distance) / 2.0f;
                        radiusSquared = radius * radius;

                        float oldToNew = distance - radius;
                        
                        center.x = (radius * center.x + oldToNew * pos.x) / distance;
                        center.y = (radius * center.y + oldToNew * pos.y) / distance;
                        center.z = (radius * center.z + oldToNew * pos.z) / distance;
                    }
                }
            }
        }
    }

    mBoundingSphere.x = center.x;
    mBoundingSphere.y = center.y;
    mBoundingSphere.z = center.z;
    mBoundingSphere.w = radius;
}
