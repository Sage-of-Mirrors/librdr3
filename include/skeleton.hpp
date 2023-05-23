#pragma once

#include "types.h"
#include "math.hpp"

#include <vector>
#include <array>
#include <string>

struct UJoint {
    std::string Name;
    UMatrix4 InverseBindMatrix;

    UVector4 Rotation;
    UVector3 Translation;
    UVector3 Scale;

    UJoint* Parent;
    std::vector<UJoint*> Children;

    std::array<float, 4> GetRotation() {
        std::array<float, 4> t;
        t[0] = Rotation.x;
        t[1] = Rotation.y;
        t[2] = Rotation.z;
        t[3] = Rotation.w;

        return t;
    }

    std::array<float, 3> GetTranslation() {
        std::array<float, 3> t;
        t[0] = Translation.x;
        t[1] = Translation.y;
        t[2] = Translation.z;

        return t;
    }

    std::array<float, 3> GetScale() {
        std::array<float, 3> t;
        t[0] = Scale.x;
        t[1] = Scale.y;
        t[2] = Scale.z;

        return t;
    }

    std::array<float, 16> GetInverseBindMatrix() {
        std::array<float, 16> t;

        t[0] = InverseBindMatrix.r0.x;
        t[1] = InverseBindMatrix.r0.y;
        t[2] = InverseBindMatrix.r0.z;
        t[3] = InverseBindMatrix.r0.w;

        t[4] = InverseBindMatrix.r1.x;
        t[5] = InverseBindMatrix.r1.y;
        t[6] = InverseBindMatrix.r1.z;
        t[7] = InverseBindMatrix.r1.w;

        t[8] = InverseBindMatrix.r2.x;
        t[9] = InverseBindMatrix.r2.y;
        t[10] = InverseBindMatrix.r2.z;
        t[11] = InverseBindMatrix.r2.w;

        t[12] = InverseBindMatrix.r3.x;
        t[13] = InverseBindMatrix.r3.y;
        t[14] = InverseBindMatrix.r3.z;
        t[15] = InverseBindMatrix.r3.w;

        return t;
    }
};

struct USkeleton {
    UJoint* Root;
    std::vector<UJoint*> FlatSkeleton;
};
