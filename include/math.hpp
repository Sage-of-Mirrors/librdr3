#pragma once

#include <cmath>

struct UVector2 { float x, y; };
struct UVector3 {
    float x, y, z;

    UVector3 operator +(const UVector3& other) {
        UVector3 sum = {
            x + other.x, y + other.y, z + other.z
        };

        return sum;
    }

    UVector3 operator -(const UVector3& other) {
        UVector3 diff = {
            x - other.x, y - other.y, z - other.z
        };

        return diff;
    }

    UVector3 operator *(float scale) {
        UVector3 prod = {
            x * scale, y * scale, z * scale
        };

        return prod;
    }

    UVector3 operator *(const UVector3& other) const {
        UVector3 prod = {
            x * other.x, y * other.y, z * other.z
        };

        return prod;
    }

    UVector3 operator /(float scale) {
        if (scale == 0.0f) {
            scale = 0.00001f;
        }
        
        UVector3 prod = {
            x / scale, y / scale, z / scale
        };

        return prod;
    }

    float length() {
        return std::sqrt((x * x) + (y * y) + (z * z));
    }

    float lengthSq() {
        return (x * x) + (y * y) + (z * z);
    }
};

struct UVector4 {
    float x, y, z, w;

    UVector4 operator +(const UVector4& other) {
        UVector4 sum = {
            x + other.x, y + other.y, z + other.z, w + other.w
        };

        return sum;
    }

    UVector4 operator *(float scale) {
        UVector4 prod = {
            x * scale, y * scale, z * scale, w * scale
        };

        return prod;
    }

    UVector4 operator /(float scale) {
        if (scale == 0.0f) {
            scale = 0.00001f;
        }

        UVector4 prod = {
            x / scale, y / scale, z / scale, w / scale
        };

        return prod;
    }

    UVector3 xyz() { return { x, y, z }; }
};

struct UMatrix3x4 {
    UVector4 r0;
    UVector4 r1;
    UVector4 r2;
};

struct UMatrix4 {
    UVector4 r0;
    UVector4 r1;
    UVector4 r2;
    UVector4 r3;
};

struct UAABB {
    UVector3 Min;
    uint32_t UnkInt;
    UVector3 Max;
    float UnkFloat;
};