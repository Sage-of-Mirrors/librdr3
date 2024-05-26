#include "level/mapdata.hpp"
#include "level/entitydef.hpp"

#include "schema/psoio.hpp"
#include "schema/schemadatablock.hpp"
#include "schema/schema.hpp"
#include "util/util.hpp"

#include <iostream>

constexpr uint32_t CMAPDATA = "CMapData"_jenkins;
constexpr uint32_t NAME = "name"_jenkins;
constexpr uint32_t PARENT = "parent"_jenkins;
constexpr uint32_t FLAGS = "flags"_jenkins;
constexpr uint32_t CONTENT_FLAGS = "contentFlags"_jenkins;
constexpr uint32_t STREAMING_EXTENTS_MIN = "streamingExtentsMin"_jenkins;
constexpr uint32_t STREAMING_EXTENTS_MAX = "streamingExtentsMax"_jenkins;
constexpr uint32_t ENTITIES_EXTENTS_MIN = "entitiesExtentsMin"_jenkins;
constexpr uint32_t ENTITIES_EXTENTS_MAX = "entitiesExtentsMax"_jenkins;
constexpr uint32_t ENTITIES = "entities"_jenkins;

using namespace rdr3::level;

CMapData::CMapData() : mNameHash(0), mParentNameHash(0), mFlags(0), mContentFlags(0) {

}

CMapData::~CMapData() {

}

bool CMapData::Deserialize(rdr3::schema::CPsoIO* pso) {
	if (pso == nullptr) {
		return false;
	}

	std::shared_ptr<rdr3::schema::CSchemaDataBlock> rootBlock = pso->GetRootDataBlock();
	if (rootBlock->GetNameHash() != CMAPDATA) {
		std::cout << "[CMapData::Deserialize] PSO's root block is not for CMapData! (Block class hash: " << rootBlock->GetNameHash() << ")" << std::endl;
		return false;
	}

	mNameHash       = rdr3::schema::GetU32(CMAPDATA, NAME,          rootBlock);
	mParentNameHash = rdr3::schema::GetU32(CMAPDATA, PARENT,        rootBlock);
	mFlags          = rdr3::schema::GetU32(CMAPDATA, FLAGS,         rootBlock);
	mContentFlags   = rdr3::schema::GetU32(CMAPDATA, CONTENT_FLAGS, rootBlock);

	mStreamingExtentsMin = rdr3::schema::GetVec3(CMAPDATA, STREAMING_EXTENTS_MIN, rootBlock);
	mStreamingExtentsMax = rdr3::schema::GetVec3(CMAPDATA, STREAMING_EXTENTS_MAX, rootBlock);
	mEntitiesExtentsMin  = rdr3::schema::GetVec3(CMAPDATA, ENTITIES_EXTENTS_MIN,  rootBlock);
	mEntitiesExtentsMax  = rdr3::schema::GetVec3(CMAPDATA, ENTITIES_EXTENTS_MAX,  rootBlock);

	uint64_t entityArrayBlockId = rdr3::schema::GetU64(CMAPDATA, ENTITIES, rootBlock);
	if (entityArrayBlockId != 0) {
		std::shared_ptr<rdr3::schema::CSchemaDataBlock> entityArrayBlock = pso->GetDataBlockById(entityArrayBlockId);

		std::vector<rdr3::schema::SSchemaArrayInfo> entityArrayInfo = rdr3::schema::GetArrrayInfo(entityArrayBlock);
		for (const rdr3::schema::SSchemaArrayInfo& info : entityArrayInfo) {
			std::shared_ptr<CEntityDef> entity = std::make_shared<CEntityDef>();
			entity->Deserialize(info.Offset, pso->GetDataBlockById(info.BlockId));

			mEntities.push_back(entity);
		}
	}

	return true;
}
