#pragma once

#include "types.h"

#include <unordered_map>

namespace rdr3 {
	namespace schema {
		class CSchemaField;
		class CSchemaDataBlock;

		using FieldHashMap = std::unordered_map<uint32_t, std::shared_ptr<CSchemaField>>;

		class CSchemaClass {
			uint32_t mNameHash;       // 0x00
			uint32_t mKey;            // 0x04
			uint32_t m0008;           // 0x08
			uint32_t m000C;           // 0x0C

			// Offset to field data is at 0x10, but we're not storing it here.
			uint32_t mDataBlockSize;  // 0x18
			uint16_t m001C;           // 0x1C
			uint16_t mFieldCount;     // 0x1E

			FieldHashMap mFields;

		public:
			CSchemaClass();
			virtual ~CSchemaClass();

			uint32_t GetNameHash() const { return mNameHash; }
			bool ContainsField(uint32_t nameHash);

			bool PsoDeserialize(bStream::CStream* stream);

			std::shared_ptr<CSchemaField> GetField(uint32_t nameHash);
		};
	}
}
