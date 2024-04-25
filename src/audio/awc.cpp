#include "audio/awc.hpp"
#include "audio/audiostreaminfo.hpp"
#include "util/bstream.h"

constexpr uint32_t AWC_MAGIC = 0x54414441;
constexpr uint32_t FLAGS_MASK = 0xFF00;

bool CAudioStreamContainer::Deserialize(bStream::CStream* data) {
	if (data == nullptr || data->readUInt32() != AWC_MAGIC)
	{
		return false;
	}

	mVersion = data->readUInt16();
	mFlags = data->readUInt16() & ~FLAGS_MASK;

	uint32_t streamCount = data->readUInt32();
	uint32_t dataOffset = data->readUInt32();

	uint16_t* chunkIndices = nullptr;
	if (GetFlag(CHUNK_IDX)) {
		chunkIndices = new uint16_t[streamCount];

		for (uint16_t i = 0; i < streamCount; i++) {
			chunkIndices[i] = data->readUInt16();
		}
	}

	// Read stream info
	mStreamInfo = std::shared_ptr<CAudioStreamInfo[]>(new CAudioStreamInfo[streamCount]);
	for (uint32_t i = 0; i < streamCount; i++) {
		mStreamInfo[i].DeserializeStreamInfo(data);
	}

	// Read chunk info for streams
	for (uint32_t i = 0; i < streamCount; i++) {
		mStreamInfo[i].DeserializeChunkInfo(data);
	}

	data->seek(dataOffset);

	for (uint32_t i = 0; i < streamCount; i++) {

	}

	if (chunkIndices != nullptr) {
		delete[] chunkIndices;
	}
}
