#pragma once

#include "types.h"

#include <cassert>

namespace rdr3 {
	namespace schema {
		class CSchemaDataBlock {
			uint32_t mNameHash;
			uint32_t mSize;
			uint64_t mOffset;

			std::unique_ptr<uint8_t[]> mData;

		public:
			CSchemaDataBlock();
			virtual ~CSchemaDataBlock();

			uint32_t GetNameHash() const { return mNameHash; }
			uint32_t GetSize() const { return mSize; }

			bool PsoDeserialize(bStream::CStream* stream);

			bStream::CMemoryStream GetDataAsStream(uint32_t offset, uint32_t size);

			template<typename T>
			T GetDataAsIntegral(uint32_t offset) {
				assert((offset % alignof(T)) == 0 && offset < mSize);
				return *reinterpret_cast<T*>(mData.get() + offset);
			}
		};
	}
}
