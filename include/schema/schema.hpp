#pragma once

#include "types.h"
#include "util/math.hpp"

#include "schemaclass.hpp"
#include "schemaenum.hpp"

#include <unordered_map>

namespace rdr3 {
	namespace schema {
		class CSchemaDataBlock;

		using SchemaClassDB = std::unordered_map<uint32_t, std::shared_ptr<CSchemaClass>>;
		using SchemaEnumDB = std::unordered_map<uint32_t, std::shared_ptr<CSchemaEnum>>;
		
		extern SchemaClassDB SchemaClassDatabase;
		extern SchemaEnumDB SchemaEnumDatabase;

		bool StoreSchemaClass(std::shared_ptr<CSchemaClass> schemaClass);
		bool StoreSchemaEnum(std::shared_ptr<CSchemaEnum> schemaEnum);

		bool IsSchemaClassLoaded(uint32_t classNameHash);
		bool DoesSchemaClassHaveField(uint32_t classNameHash, uint32_t fieldNameHash);

		bool      GetBool(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		int8_t    GetS8(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		uint8_t   GetU8(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		int16_t   GetS16(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		uint16_t  GetU16(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		int32_t   GetS32(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		uint32_t  GetU32(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		int64_t   GetS64(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		uint64_t  GetU64(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		float     GetF32(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		double    GetF64(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		Vector3   GetVec3(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		Vector4   GetVec4(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);
		Matrix3x4 GetMat34(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset = 0);

		struct SSchemaArrayInfo {
			uint8_t BlockId;
			uint8_t Field0001;
			uint8_t Field0002;
			uint16_t Offset;
			uint8_t Field0005;
			uint8_t Field0006;
			uint8_t Field0007;

			bool Deserialize(bStream::CStream* stream);
		};

		std::vector<SSchemaArrayInfo> GetArrrayInfo(std::shared_ptr<CSchemaDataBlock> data);
	}
}
