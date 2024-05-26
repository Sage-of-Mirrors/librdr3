#include "schema/schemaenum.hpp"
#include "util/bstream.h"
#include "util/streamutil.hpp"

using namespace rdr3::schema;

CSchemaEnum::CSchemaEnum() {

}

CSchemaEnum::~CSchemaEnum() {

}

bool CSchemaEnum::PsoDeserialize(bStream::CStream* stream) {
	mNameHash = stream->readUInt32();
	m0004 = stream->readUInt32();
	mOffset = StreamUtil::DeserializePtr64(stream);
	mCount = stream->readUInt32();
	m0014 = stream->readUInt32();

	size_t curPos = stream->tell();
	stream->seek(mOffset);

	for (uint32_t i = 0; i < mCount; i++) {
		mValues.insert(std::pair<uint32_t, uint32_t>(stream->readUInt32(), stream->readUInt32()));
	}

	stream->seek(curPos);
	return true;
}
