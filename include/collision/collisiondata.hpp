#pragma once

#include "types.h"
#include "util/math.hpp"

enum class ECollisionType : uint8_t {
	SPHERE,
	CAPSULE,
	TAPERED_CAPSULE,
	BOX,
	GEOMETRY,
	BVH,
	COMPOSITE,
	TRIANGLE,
	DISC,
	CYLINDER,
	PLANE,
	MORPH_GEOMETRY,
	RING,
	CLOTH
};

class CBlockMap;

struct CCollisionData {
	                               // 0x0000 is for the vtable pointer, loaded at runtime
	CBlockMap* mBlockMap;          // 0x0008

	ECollisionType mType;          // 0x0010
	uint8_t m0011;                 // 0x0011
	uint8_t m0012;                 // 0x0012
	uint8_t m0013;                 // 0x0013

	float mBoundingSphereRadius;   // 0x0014
	uint32_t m0018;                // 0x0018
	uint32_t m001C;                // 0x001C

	Vector3 mBoundingBoxMax;       // 0x0020
	float mMargin;                 // 0x002C
	
	Vector3 mBoundingBoxMin;       // 0x0030
	uint32_t m003C;                // 0x003C
	
	Vector3 mBoundingBoxCenter;    // 0x0040
	uint32_t mMaterial_1;          // 0x004C

	Vector3 mBoundingSphereCenter; // 0x0050
	uint32_t mMaterial_2;          // 0x005C

	Vector3 mInertia;              // 0x0060
	float mMass;                   // 0x006C
};
