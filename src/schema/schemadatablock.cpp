#include "schema/schemadatablock.hpp"
#include "util/bstream.h"
#include "util/streamutil.hpp"

using namespace rdr3::schema;

CSchemaDataBlock::CSchemaDataBlock() : mNameHash(0), mSize(0), mOffset(0), mData(nullptr) {

}

CSchemaDataBlock::~CSchemaDataBlock() {
}

bStream::CMemoryStream CSchemaDataBlock::GetDataAsStream(uint32_t offset, uint32_t size) {
	assert(offset < mSize && (offset + size) <= mSize);

	bStream::CMemoryStream strm(size, bStream::Little, bStream::In);
	std::memcpy(strm.getBuffer(), mData.get() + offset, size);

	return strm;
}

bool CSchemaDataBlock::PsoDeserialize(bStream::CStream* stream) {
	mNameHash = stream->readUInt32();
	mSize = stream->readUInt32();
	mOffset = StreamUtil::DeserializePtr64(stream);

	size_t curPos = stream->tell();
	stream->seek(mOffset);

	mData = std::make_unique<uint8_t[]>(mSize);
	stream->readBytesTo(mData.get(), mSize);

	stream->seek(curPos);
	return true;
}
