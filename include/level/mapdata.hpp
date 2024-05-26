#pragma once

#include "types.h"
#include "util/math.hpp"

namespace rdr3 {
	namespace schema {
		class CPsoIO;
	}

	namespace level {
		class CEntityDef;

		class CMapData {
			uint32_t mNameHash;
			uint32_t mParentNameHash;
			uint32_t mFlags;
			uint32_t mContentFlags;

			Vector3 mStreamingExtentsMin;
			Vector3 mStreamingExtentsMax;
			Vector3 mEntitiesExtentsMin;
			Vector3 mEntitiesExtentsMax;

			shared_vector<CEntityDef> mEntities;
			// shared_vector<CContainerLodDef> mContainerLods;
			// shared_vector<CBoxOccluder> mBoxOccluders;
			// shared_vector<CModelOccluder> mModelOccluders;
			// shared_vector<std::string> mPhysicsDictionaries;
			// CInstancedMapData mInstancedData;

			// shared_vector<CTimeCycleModifier> mTimeCycleModifiers;
			// shared_vector<CTimeCycleVolume> mTimeCycleVolumes;
			// shared_vector<CReflectionProbeData> mReflectionProbes;
			// shared_vector<CCarGen> mCarGenerators;
			// CLODLight mLodLights;
			// CDistantLodLight mDistantLodLights;
			// CBlockDesc mBlock;

		public:
			CMapData();
			virtual ~CMapData();

			bool Deserialize(schema::CPsoIO* pso);
		};
	}
}
