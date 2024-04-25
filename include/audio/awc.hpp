#pragma once

#include "types.h"

enum EStreamContainerFlag : uint8_t {
	CHUNK_IDX = 0x01,
	SINGLE_CHAN_ENCRYPT = 0x02,
	MULTI_CHAN = 0x04,
	MULTI_CHAN_ENCRYPT = 0x08
};

struct CAudioStreamInfo;

struct CAudioStreamContainer {
	uint16_t mVersion;
	uint16_t mFlags;

	std::shared_ptr<CAudioStreamInfo[]> mStreamInfo;

	bool Deserialize(bStream::CStream* data);

	bool GetFlag(EStreamContainerFlag flag) const { return (mFlags & flag) != 0; }
	void SetFlag(EStreamContainerFlag flag, bool value) { mFlags &= ~flag; mFlags |= (value << flag); }
};
