#include "schema/psoio.hpp"
#include "schema/schema.hpp"
#include "schema/schemaclass.hpp"

#include "util/util.hpp"
#include "util/bstream.h"
#include "util/streamutil.hpp"

using namespace rdr3::schema;

constexpr uint32_t PSO_META_MAGIC = 0x50524430;
constexpr uint32_t PC_PLATFORM_ID = 0x79;

struct SPsoHeader {
	uint64_t VTablePtr;        // 0x00
	uint64_t BlockMapPtr;      // 0x08

	uint32_t Magic;            // 0x10
	uint32_t PlatformId;       // 0x14
	uint64_t field_0018;       // 0x18

	uint64_t mRootBlockId;     // 0x20

	uint64_t ClassesOffset;    // 0x28
	uint64_t EnumsOffset;      // 0x30
	uint64_t DataBlocksOffset; // 0x38
	uint64_t field_0040;       // 0x40

	uint16_t ClassCount;       // 0x48
	uint16_t EnumCount;        // 0x4A
	uint16_t DataBlockCount;   // 0x4C
	uint16_t field_004E;       // 0x4E

	uint64_t field_0050;       // 0x50
	uint64_t field_0058;       // 0x58

	bool Deserialize(bStream::CStream* stream) {
		VTablePtr = stream->readUInt64();
		BlockMapPtr = StreamUtil::DeserializePtr64(stream);

		Magic = stream->readUInt32();
		if (Magic != PSO_META_MAGIC) {
			return false;
		}

		PlatformId = stream->readUInt32();
		if (PlatformId != PC_PLATFORM_ID) {
			return false;
		}

		field_0018 = stream->readUInt64();
		mRootBlockId = stream->readUInt64();

		ClassesOffset = StreamUtil::DeserializePtr64(stream);
		EnumsOffset = StreamUtil::DeserializePtr64(stream);
		DataBlocksOffset = StreamUtil::DeserializePtr64(stream);
		field_0040 = StreamUtil::DeserializePtr64(stream);

		ClassCount = stream->readUInt16();
		EnumCount = stream->readUInt16();
		DataBlockCount = stream->readUInt16();
		field_004E = stream->readUInt16();

		field_0050 = StreamUtil::DeserializePtr64(stream);
		field_0058 = stream->readUInt64();

		return true;
	}
};

/* === CPsoIO === */

CPsoIO::CPsoIO() : mRootBlockId(0) {

}

CPsoIO::~CPsoIO() {

}

bool CPsoIO::ReadClasses(bStream::CStream* stream, size_t offset, uint16_t count) {
	stream->seek(offset);

	for (uint16_t i = 0; i < count; i++) {
		std::shared_ptr<CSchemaClass> newClass = std::make_shared<CSchemaClass>();
		newClass->PsoDeserialize(stream);

		StoreSchemaClass(newClass);
	}

	return true;
}

bool CPsoIO::ReadEnums(bStream::CStream* stream, size_t offset, uint16_t count) {
	stream->seek(offset);

	for (uint16_t i = 0; i < count; i++) {
		std::shared_ptr<CSchemaEnum> newEnum = std::make_shared<CSchemaEnum>();
		newEnum->PsoDeserialize(stream);

		StoreSchemaEnum(newEnum);
	}

	return true;
}

bool CPsoIO::ReadDataBlocks(bStream::CStream* stream, size_t offset, uint16_t count) {
	stream->seek(offset);

	for (uint16_t i = 0; i < count; i++) {
		std::shared_ptr<CSchemaDataBlock> newBlock = std::make_shared<CSchemaDataBlock>();
		newBlock->PsoDeserialize(stream);

		mDataBlocks.push_back(newBlock);
	}

	return true;
}

void CPsoIO::Deserialize(bStream::CStream* stream) {
	SPsoHeader header;
	header.Deserialize(stream);

	mRootBlockId = header.mRootBlockId;

	if (header.ClassCount != 0) {
		ReadClasses(stream, header.ClassesOffset, header.ClassCount);
	}
	if (header.EnumCount != 0) {
		ReadEnums(stream, header.EnumsOffset, header.EnumCount);
	}
	if (header.DataBlockCount != 0) {
		ReadDataBlocks(stream, header.DataBlocksOffset, header.DataBlockCount);
	}
}
