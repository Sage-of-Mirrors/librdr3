#pragma once

#include "types.h"
#include "util/math.hpp"

#include <vector>
#include <array>
#include <string>
#include <memory>

struct CJoint {
    std::string Name;
    Matrix4 InverseBindMatrix;
    Matrix4 BindMatrix;

    Vector4 Rotation;
    Vector3 Translation;
    Vector3 Scale;

    std::weak_ptr<CJoint> Parent;
    shared_vector<CJoint> Children;

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

    std::array<std::array<float, 4>, 4> GetInverseBindMatrix() {
        std::array<float, 4> r0;
        std::array<float, 4> r1;
        std::array<float, 4> r2;
        std::array<float, 4> r3;

        r0[0] = InverseBindMatrix.r0.x;
        r0[1] = InverseBindMatrix.r0.y;
        r0[2] = InverseBindMatrix.r0.z;
        r0[3] = InverseBindMatrix.r0.w;

        r1[0] = InverseBindMatrix.r1.x;
        r1[1] = InverseBindMatrix.r1.y;
        r1[2] = InverseBindMatrix.r1.z;
        r1[3] = InverseBindMatrix.r1.w;

        r2[0] = InverseBindMatrix.r2.x;
        r2[1] = InverseBindMatrix.r2.y;
        r2[2] = InverseBindMatrix.r2.z;
        r2[3] = InverseBindMatrix.r2.w;

        r3[0] = InverseBindMatrix.r3.x;
        r3[1] = InverseBindMatrix.r3.y;
        r3[2] = InverseBindMatrix.r3.z;
        r3[3] = InverseBindMatrix.r3.w;

        return std::array<std::array<float, 4>, 4> {r0, r1, r2, r3};
    }

    std::array<std::array<float, 4>, 4> GetBindMatrix() {
        std::array<float, 4> r0;
        std::array<float, 4> r1;
        std::array<float, 4> r2;
        std::array<float, 4> r3;

        r0[0] = BindMatrix.r0.x;
        r0[1] = BindMatrix.r0.y;
        r0[2] = BindMatrix.r0.z;
        r0[3] = BindMatrix.r0.w;

        r1[0] = BindMatrix.r1.x;
        r1[1] = BindMatrix.r1.y;
        r1[2] = BindMatrix.r1.z;
        r1[3] = BindMatrix.r1.w;

        r2[0] = BindMatrix.r2.x;
        r2[1] = BindMatrix.r2.y;
        r2[2] = BindMatrix.r2.z;
        r2[3] = BindMatrix.r2.w;

        r3[0] = BindMatrix.r3.x;
        r3[1] = BindMatrix.r3.y;
        r3[2] = BindMatrix.r3.z;
        r3[3] = BindMatrix.r3.w;

        return std::array<std::array<float, 4>, 4> {r0, r1, r2, r3};
    }
};

struct CSkeleton {
    std::weak_ptr<CJoint> Root;
    shared_vector<CJoint> FlatSkeleton;
};
