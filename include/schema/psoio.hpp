#pragma once

#include "types.h"

#include "schemaclass.hpp"
#include "schemafield.hpp"
#include "schemaenum.hpp"
#include "schemadatablock.hpp"
#include "schema.hpp"

namespace rdr3 {
	namespace schema {
		class CPsoIO {
			uint32_t mRootBlockId;
			shared_vector<CSchemaDataBlock> mDataBlocks;

			bool ReadClasses(bStream::CStream* stream, size_t offset, uint16_t count);
			bool ReadEnums(bStream::CStream* stream, size_t offset, uint16_t count);
			bool ReadDataBlocks(bStream::CStream* stream, size_t offset, uint16_t count);

		public:
			CPsoIO();
			virtual ~CPsoIO();

			std::shared_ptr<CSchemaDataBlock> GetDataBlockByIndex(uint32_t idx) const { return mDataBlocks[idx]; }
			std::shared_ptr<CSchemaDataBlock> GetDataBlockById(uint32_t id) const { return mDataBlocks[id - 1]; }
			std::shared_ptr<CSchemaDataBlock> GetRootDataBlock() const { return GetDataBlockById(mRootBlockId); }

			void Deserialize(bStream::CStream* stream);
		};
	}
}
