#pragma once

#include <cmath>

struct Vector2 { float x, y; };

struct Vector3 {
    float x, y, z;

    Vector3 operator +(const Vector3& other) {
        Vector3 sum = {
            x + other.x, y + other.y, z + other.z
        };

        return sum;
    }

    Vector3 operator -(const Vector3& other) {
        Vector3 diff = {
            x - other.x, y - other.y, z - other.z
        };

        return diff;
    }

    Vector3 operator *(float scale) {
        Vector3 prod = {
            x * scale, y * scale, z * scale
        };

        return prod;
    }

    Vector3 operator *(const Vector3& other) const {
        Vector3 prod = {
            x * other.x, y * other.y, z * other.z
        };

        return prod;
    }

    Vector3 operator /(float scale) {
        if (scale == 0.0f) {
            scale = 0.00001f;
        }
        
        Vector3 prod = {
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

    Vector3 cross(const Vector3& other) {
        Vector3 prod;
        prod.x = (y * other.z) - (z * other.y);
        prod.y = (z * other.x) - (x * other.z);
        prod.z = (x * other.y) - (y * other.x);

        return prod;
    }

    Vector3 normalized() {
        float len = length();

        return {
            x / len, y / len, z / len
        };
    }

    Vector3 toZUp() {
        return { x, z, -y };
    }
};

struct Vector4 {
    float x, y, z, w;

    Vector4 operator +(const Vector4& other) {
        Vector4 sum = {
            x + other.x, y + other.y, z + other.z, w + other.w
        };

        return sum;
    }

    Vector4 operator *(float scale) {
        Vector4 prod = {
            x * scale, y * scale, z * scale, w * scale
        };

        return prod;
    }

    Vector4 operator /(float scale) {
        if (scale == 0.0f) {
            scale = 0.00001f;
        }

        Vector4 prod = {
            x / scale, y / scale, z / scale, w / scale
        };

        return prod;
    }

    Vector4 xyz() { return { x, y, z }; }
};

struct Matrix3x4 {
    Vector4 r0;
    Vector4 r1;
    Vector4 r2;
};

struct Matrix4 {
    Vector4 r0;
    Vector4 r1;
    Vector4 r2;
    Vector4 r3;
};

struct AABB {
    Vector3 Min;
    uint32_t UnkInt;
    Vector3 Max;
    float UnkFloat;
};