#pragma once

#include "types.h"
#include "util/math.hpp"

namespace rdr3 {
	namespace schema {
		class CSchemaDataBlock;
	}

	namespace level {
		class CEntityDef {
			uint32_t mArchetypeNameHash;
			uint32_t mFlags;
			uint64_t mGUID;

			Vector3 mPosition;
			Vector4 mRotation;
			Vector3 mScale;

			int32_t mParentIndex;

			float mLodDistance;
			float mChildLodDistance;
			uint32_t mLodLevel;

			uint32_t mChildCount;
			uint32_t mPriorityLevel;
			//shared_vector<CExtensionDef> mExtensions;
			uint32_t mPowerGridId;

			uint32_t mTintValue;
			uint8_t m0084;
			uint8_t m0085;

		public:
			CEntityDef();
			virtual ~CEntityDef();

			bool Deserialize(uint32_t offset, std::shared_ptr<rdr3::schema::CSchemaDataBlock> block);
		};
	}
}
