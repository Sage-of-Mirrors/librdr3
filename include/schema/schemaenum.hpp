#pragma once

#include "types.h"

#include <unordered_map>

namespace rdr3 {
	namespace schema {
		using EnumHashMap = std::unordered_map<uint32_t, uint32_t>;

		class CSchemaEnum {
			uint32_t mNameHash; // 0x00
			uint32_t m0004;     // 0x04
			uint64_t mOffset;   // 0x08
			uint32_t mCount;    // 0x10
			uint32_t m0014;     // 0x14

			EnumHashMap mValues;

		public:
			CSchemaEnum();
			virtual ~CSchemaEnum();

			uint32_t GetNameHash() const { return mNameHash; }

			bool PsoDeserialize(bStream::CStream* stream);
		};
	}
}
