#pragma once

#include "types.h"

enum class EAudioChunkType : uint8_t
{
    //should be the last byte of the hash of the name.
    DATA          = 0x55, // 0x5EB5E655
    FORMAT        = 0xFA, // 0x6061D4FA
    ANIM          = 0x5C, // 0x938C925C not correct
    PEAK          = 0x36, // 0x8B946236
    MIDI          = 0x68, // 0x71DE4C68
    GESTURE       = 0x2B, // 0x23097A2B
    GRAINS        = 0x5A, // 0xE787895A
    LOOPS         = 0xD9, // 0x252C20D9
    MARKERS       = 0xBD, // 0xD4CB98BD
    STRM_FORMAT   = 0x48, // 0x81F95048
    SEEK_TABLE    = 0xA3, // 0x021E86A3
    VORBIS_HEADER = 0x7F, // 0x20D0EF7F
    UNK_1         = 0x76, // Seems to be info about the track 
};

struct CAudioChunkInfo;

struct CAudioStreamInfo : public std::enable_shared_from_this<CAudioChunkInfo> {
	uint32_t mChunkCount;
	uint32_t mId;

	std::shared_ptr<CAudioChunkInfo[]> mChunkInfo;

    bool SerializeStreamInfo(bStream::CStream* data);
	bool DeserializeStreamInfo(bStream::CStream* data);

    bool SerializeChunkInfo(bStream::CStream* data);
    bool DeserializeChunkInfo(bStream::CStream* data);
};

struct CAudioChunkInfo : public std::enable_shared_from_this<CAudioChunkInfo> {
    EAudioChunkType mType;
    uint32_t mSize;
    uint32_t mOffset;

    bool Serialize(bStream::CStream* data);
    bool Deserialize(bStream::CStream* data);
};
