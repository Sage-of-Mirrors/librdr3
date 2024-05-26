#include "schema/schemafield.hpp"
#include "util/bstream.h"

using namespace rdr3::schema;

/* === CSchemaField === */

CSchemaField::CSchemaField() : mNameHash(0), mOffset(0), mType(ESchemaFieldType::FTP_NONE), mAuxFieldIndex(0), mRefTypeIndex(0), mAuxTypeNameHash(0) {

}

CSchemaField::~CSchemaField() {

}

bool CSchemaField::PsoDeserialize(bStream::CStream* stream) {
	mNameHash = stream->readUInt32();
	mOffset = stream->readUInt32();
	mType = (ESchemaFieldType)stream->readUInt8();
	mAuxFieldIndex = stream->readUInt8();
	mRefTypeIndex = stream->readUInt16();
	mAuxTypeNameHash = stream->readUInt32();

	return true;
}
