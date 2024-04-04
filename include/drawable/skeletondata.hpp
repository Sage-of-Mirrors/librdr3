#pragma once

#include "types.h"
#include "util/math.hpp"

#include <vector>
#include <string>

struct CJoint;
struct CSkeleton;

class CJointData {
    Vector4 mRotation;
    Vector4 mTranslation;
    Vector4 mScale;

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
    CJointData();
    virtual ~CJointData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    uint16_t GetParentIndex() const { return mParentIndex; }
    uint16_t GetNextSiblingIndex() const { return mNextSiblingIndex; }

    std::shared_ptr<CJoint> GetJoint();
};

class CSkeletonData {
    uint64_t mVTable; // 0x00
    uint64_t m0008;   // 0x08

    uint32_t m001C;

    shared_vector<CJointData> mJoints;

    std::vector<Matrix4> mInverseBindMatrices;
    std::vector<Matrix4> mBindMatrices;

    std::vector<uint16_t> mIndices;

    uint16_t mExternalParentJointTag; // This is the tag of this skeleton's parent joint in another skeleton, e.g. fragments.

public:
    CSkeletonData();
    virtual ~CSkeletonData();

    void Deserialize(bStream::CStream* stream);
    void Serialize(bStream::CStream* stream);

    std::shared_ptr<CSkeleton> GetSkeleton();
};
