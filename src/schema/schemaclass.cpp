#include "schema/schemaclass.hpp"
#include "schema/schemafield.hpp"
#include "util/bstream.h"
#include "util/streamutil.hpp"

constexpr uint32_t AUX_FIELD_NAME = 0x0100;

using namespace rdr3::schema;

CSchemaClass::CSchemaClass() : mNameHash(0), mKey(0), m0008(0), m000C(0), mDataBlockSize(0), m001C(0), mFieldCount(0) {

}

CSchemaClass::~CSchemaClass() {

}

bool CSchemaClass::ContainsField(uint32_t nameHash) {
	return GetField(nameHash) != nullptr;
}

bool CSchemaClass::PsoDeserialize(bStream::CStream* stream) {
	mNameHash = stream->readUInt32();
	mKey = stream->readUInt32();
	m0008 = stream->readUInt32();
	m000C = stream->readUInt32();

	uint64_t fieldsOffset = StreamUtil::DeserializePtr64(stream);
	mDataBlockSize = stream->readUInt32();
	m001C = stream->readUInt16();
	mFieldCount = stream->readUInt16();

	size_t curPos = stream->tell();
	stream->seek(fieldsOffset);

	for (uint16_t i = 0; i < mFieldCount; i++) {
		// Skip fields with this particular name hash, they're metadata for other fields.
		if (stream->peekUInt32(stream->tell()) == AUX_FIELD_NAME) {
			stream->skip(0x10);
			continue;
		}

		std::shared_ptr<CSchemaField> newField = std::make_shared<CSchemaField>();
		newField->PsoDeserialize(stream);

		mFields.insert(std::pair<uint32_t, std::shared_ptr<CSchemaField>>(newField->GetNameHash(), newField));
	}

	stream->seek(curPos);
	return true;
}

std::shared_ptr<CSchemaField> CSchemaClass::GetField(uint32_t nameHash) {
	auto it = mFields.find(nameHash);
	if (it == mFields.end()) {
		return nullptr;
	}

	return mFields[nameHash];
}
