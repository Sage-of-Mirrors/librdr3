#pragma once

#include "types.h"
#include "math.hpp"

#include <vector>
#include <string>

struct UJoint;
struct USkeleton;

class UJointData {
    UVector4 mRotation;
    UVector4 mTranslation;
    UVector4 mScale;

    std::string mName;

    uint64_t m0038;

    uint16_t m0040;
    uint16_t m0042;

    uint16_t mNextSiblingIndex;
    uint16_t mParentIndex;
    uint16_t mCurrentIndex;

    uint16_t mTag;

    uint16_t m004C;
    uint16_t m004E;

public:
    UJointData();
    virtual ~UJointData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    uint16_t GetParentIndex() const { return mParentIndex; }
    uint16_t GetNextSiblingIndex() const { return mNextSiblingIndex; }

    UJoint* GetJoint();
};

class USkeletonData {
    uint64_t mVTable; // 0x00
    uint64_t m0008;   // 0x08

    uint32_t m001C;

    std::vector<UJointData*> mJoints;

    std::vector<UMatrix4> mInverseBindMatrices;
    std::vector<UMatrix4> mBindMatrices;

    std::vector<uint16_t> mIndices;

    uint16_t mExternalParentJointTag; // This is the tag of this skeleton's parent joint in another skeleton, e.g. fragments.

public:
    USkeletonData();
    virtual ~USkeletonData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    USkeleton* GetSkeleton();
    UJoint* BuildJointHierarchy(UJointData* current, USkeleton* skeleton);
};
