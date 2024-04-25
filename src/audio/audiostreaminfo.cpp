#include "audio/audiostreaminfo.hpp"
#include "util/bstream.h"

#include <iostream>

/* CAudioStreamInfo */

bool CAudioStreamInfo::SerializeStreamInfo(bStream::CStream* data) {
	uint32_t bitField = 0;
	bitField |= mChunkCount << 0x1D;
	bitField |= mId & 0x1FFFFFFF;

	data->writeUInt32(bitField);
	return true;
}

bool CAudioStreamInfo::DeserializeStreamInfo(bStream::CStream* data) {
	uint32_t bitField = data->readUInt32();

	mChunkCount = (uint32_t)(bitField >> 0x1D);
	mId = (uint32_t)(bitField & 0x1FFFFFFF);

	mChunkInfo = std::shared_ptr<CAudioChunkInfo[]>(new CAudioChunkInfo[mChunkCount]);

	return true;
}

bool CAudioStreamInfo::SerializeChunkInfo(bStream::CStream* data) {
	for (uint32_t i = 0; i < mChunkCount; i++) {
		mChunkInfo[i].Serialize(data);
	}

	return true;
}

bool CAudioStreamInfo::DeserializeChunkInfo(bStream::CStream* data) {
	for (uint32_t i = 0; i < mChunkCount; i++) {
		mChunkInfo[i].Deserialize(data);
		std::cout << std::to_string((uint8_t)mChunkInfo[i].mType) << std::endl;
	}

	return true;
}

/* CAudioChunkInfo */

bool CAudioChunkInfo::Serialize(bStream::CStream* data) {
	uint64_t bitField = 0;
	bitField |= (uint8_t)(mType) << 0x38;
	bitField |= (mSize & 0x0FFFFFFF) << 0x1C;
	bitField |= (mOffset & 0x0FFFFFFF);

	data->writeUInt64(bitField);
	return true;
}

bool CAudioChunkInfo::Deserialize(bStream::CStream* data) {
	uint64_t bitField = data->readUInt64();

	mType   = (EAudioChunkType)(bitField >> 0x38);
	mSize   = (uint32_t)((bitField >> 0x1C) & 0x0FFFFFFF);
	mOffset = (uint32_t)(bitField & 0x0FFFFFFF);

	return true;
}
