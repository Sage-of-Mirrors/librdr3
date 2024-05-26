#include "level/entitydef.hpp"
#include "schema/schema.hpp"
#include "schema/schemadatablock.hpp"
#include "util/util.hpp"

constexpr uint32_t ENTITY_DEF_SIZE = 0x90;

constexpr uint32_t CENTITYDEF = "CEntityDef"_jenkins;
constexpr uint32_t ARCHETYPE_NAME = "archetypeName"_jenkins;
constexpr uint32_t FLAGS = "flags"_jenkins;
constexpr uint32_t GUID = "id"_jenkins;
constexpr uint32_t POSITION = "position"_jenkins;
constexpr uint32_t ROTATION = "rotation"_jenkins;
constexpr uint32_t SCALE_XY = "scaleXY"_jenkins;
constexpr uint32_t SCALE_Z = "scaleZ"_jenkins;
constexpr uint32_t PARENT_INDEX = "parentIndex"_jenkins;
constexpr uint32_t LOD_DIST = "lodDist"_jenkins;
constexpr uint32_t CHILD_LOD_DIST = "childLodDist"_jenkins;
constexpr uint32_t LOD_LEVEL = "lodLevel"_jenkins;
constexpr uint32_t NUM_CHILDREN = "numChildren"_jenkins;
constexpr uint32_t PRIORITY_LEVEL = "priorityLevel"_jenkins;
constexpr uint32_t EXTENSIONS = "extensions"_jenkins;
constexpr uint32_t POWER_GRID_ID = "powerGridId"_jenkins;
constexpr uint32_t TINT_VALUE = "tintValue"_jenkins;
constexpr uint32_t FIELD_0084 = 0x0DC5F270;
constexpr uint32_t FIELD_0085 = 0x540CA436;

using namespace rdr3::level;

CEntityDef::CEntityDef() {

}

CEntityDef::~CEntityDef() {

}

bool CEntityDef::Deserialize(uint32_t offset, std::shared_ptr<rdr3::schema::CSchemaDataBlock> block) {
	mArchetypeNameHash = rdr3::schema::GetU32(CENTITYDEF, ARCHETYPE_NAME, block, offset);
	mFlags = rdr3::schema::GetU32(CENTITYDEF, FLAGS, block, offset);
	mGUID = rdr3::schema::GetU64(CENTITYDEF, GUID, block, offset);

	mPosition = rdr3::schema::GetVec3(CENTITYDEF, POSITION, block, offset);
	mRotation = rdr3::schema::GetVec4(CENTITYDEF, ROTATION, block, offset);

	mScale.x = rdr3::schema::GetF32(CENTITYDEF, SCALE_XY, block, offset);
	mScale.y = mScale.x;
	mScale.z = rdr3::schema::GetF32(CENTITYDEF, SCALE_Z, block, offset);

	mParentIndex = rdr3::schema::GetS32(CENTITYDEF, PARENT_INDEX, block, offset);

	mLodDistance = rdr3::schema::GetF32(CENTITYDEF, LOD_DIST, block, offset);
	mChildLodDistance = rdr3::schema::GetF32(CENTITYDEF, CHILD_LOD_DIST, block, offset);

	mLodLevel = rdr3::schema::GetU32(CENTITYDEF, LOD_LEVEL, block, offset);
	mChildCount = rdr3::schema::GetU32(CENTITYDEF, NUM_CHILDREN, block, offset);
	mPriorityLevel = rdr3::schema::GetU32(CENTITYDEF, PRIORITY_LEVEL, block, offset);

	uint64_t extensionsArrayBlockId = rdr3::schema::GetU64(CENTITYDEF, EXTENSIONS, block, offset);
	if (extensionsArrayBlockId != 0) {
		// TODO: Load extensions
	}

	mPowerGridId = rdr3::schema::GetU32(CENTITYDEF, POWER_GRID_ID, block, offset);

	mTintValue = rdr3::schema::GetU32(CENTITYDEF, TINT_VALUE, block, offset);
	m0084 = rdr3::schema::GetU8(CENTITYDEF, FIELD_0084, block, offset);
	m0085 = rdr3::schema::GetU8(CENTITYDEF, FIELD_0085, block, offset);

	return true;
}
