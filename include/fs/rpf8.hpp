// Adapted from https://github.com/0x1F9F1/Swage/blob/master/src/games/rage/rpf8.h

#pragma once

#include "types.h"

#include <filesystem>

namespace rdr3 {
	namespace fs {
		class CFSDevice;
	}
}

constexpr uint8_t DIR_FILE_EXT = 0xFE;
constexpr uint8_t ENCRYPTION_KEY_NONE = 0xFF;

enum class ECompressionType : uint8_t {
	CMP_NONE,
	CMP_DEFLATE,
	CMP_OODLE
};

struct CRPF8Header {
	uint32_t mMagic;
	uint32_t mEntryCount;
	uint32_t mNamesLength;
	uint16_t mDecryptionTag;
	uint16_t mPlatformId;

	bool Deserialize(bStream::CStream* stream);
};

struct CRPF8Entry {
	// FileHash:32
	// EncryptionConfig:8
	// EncryptionKeyId:8
	// FileExtId:8
	// IsResource:1
	// IsSignatureProtected:1
	uint64_t mBitfield0;

	// OnDiskSize:28
	// Offset:31
	// CompresorId:5
	uint64_t mBitfield1;

	// rage::datResourceInfo
	// ResIdUpper:4
	// Size1:28
	// ResIdLower:4
	// Size2:28
	uint64_t mBitfield2;

	bool Deserialize(bStream::CStream* stream);

	uint32_t GetHash() const { return mBitfield0 & 0xFFFFFFFF; }
	uint8_t GetEncryptionConfig() const { return (mBitfield0 >> 0x20) & 0xFF; }
	uint8_t GetEncryptionKeyId()  const { return (mBitfield0 >> 0x28) & 0xFF; }
	uint8_t GetFileExtId() const { return (mBitfield0 >> 0x30) & 0xFF; }
	bool IsResource() const { return (mBitfield0 >> 0x38) & 0x01; }
	bool IsSignatureProtected() const { return (mBitfield0 >> 0x38) & 0x02; }

	uint32_t GetOnDiskSize() const { return (mBitfield1 & 0xFFFFFFF) << 4; }
	uint64_t GetOffset() const { return ((mBitfield1 >> 0x1C) & 0x7FFFFFFF) << 4; }
	ECompressionType GetCompressorId() const { return ECompressionType((mBitfield1 >> 0x3B) & 0x1F); }
	bool IsDirectory() const { return GetFileExtId() == DIR_FILE_EXT; }
	bool IsEncrypted() const { return GetEncryptionKeyId() != ENCRYPTION_KEY_NONE; }

	uint32_t GetVirtualFlags() const { return mBitfield2 & 0xFFFFFFFF; }
	uint32_t GetPhysicalFlags() const { return mBitfield2 >> 32; }
	uint32_t GetVirtualSize() const { return mBitfield2 & 0xFFFFFFF0; }
	uint32_t GetPhysicalSize() const { return (mBitfield2 >> 32) & 0xFFFFFFF0; }

    uint8_t GetResourceId() const {
		uint8_t upper = (mBitfield2 & 0xF);
		uint8_t lower = (mBitfield2 >> 32) & 0xF;

        return (upper << 4) | lower;
    }

    uint64_t GetSize() const {
		if (IsDirectory()) {
			return 0;
		}

        if (IsResource()) {
			uint64_t virt_size = GetVirtualSize();
			uint64_t phys_size = GetPhysicalSize();

            return virt_size + phys_size;
        }
        else {
            return mBitfield2;
        }
    }

    //bool GetResourceFileHeader(datResourceFileHeader& info) const;
};

std::shared_ptr<class rdr3::fs::CFSDevice> LoadRPF8(std::filesystem::path filePath);
