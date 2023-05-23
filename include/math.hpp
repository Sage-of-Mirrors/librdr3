#pragma once

struct UVector2 { float x, y; };
struct UVector3 { float x, y, z; };
struct UVector4 { float x, y, z, w; };

struct UMatrix4 {
    UVector4 r0;
    UVector4 r1;
    UVector4 r2;
    UVector4 r3;
};