#include "fs/rpf8.hpp"
#include "fs/fsdevice.hpp"

#include "crypto/crypto.hpp"
#include "crypto/keys.hpp"
#include "crypto/tfit2.hpp"

#include "util/bstream.h"

constexpr uint32_t RSA_SIG_SIZE = 0x100;

/* CRPF8Header */

bool CRPF8Header::Deserialize(bStream::CStream* stream) {
	mMagic = stream->readUInt32();
	if (mMagic != 0x52504638) {
		return false;
	}

	mEntryCount = stream->readUInt32();
	mNamesLength = stream->readUInt32();
	mDecryptionTag = stream->readUInt16();
	mPlatformId = stream->readUInt16();

	if (mPlatformId != 0x79) {
		return false;
	}

	return true;
}

/* CRPF8Entry */

bool CRPF8Entry::Deserialize(bStream::CStream* stream) {
	mBitfield0 = stream->readUInt64();
	mBitfield1 = stream->readUInt64();
	mBitfield2 = stream->readUInt64();

	return true;
}

std::shared_ptr<rdr3::fs::CFSDevice> LoadRPF8(std::filesystem::path filePath) {
	std::unique_ptr<bStream::CStream> stream = std::make_unique<bStream::CFileStream>(filePath.u8string());

	CRPF8Header header;
	header.Deserialize(stream.get());

	uint8_t rsaSig[RSA_SIG_SIZE];
	stream->readBytesTo(rsaSig, RSA_SIG_SIZE);

	std::vector<CRPF8Entry> entries;
	entries.reserve(header.mEntryCount);

	for (uint32_t i = 0; i < header.mEntryCount; i++) {
		CRPF8Entry entry;
		entry.Deserialize(stream.get());

		entries.push_back(entry);
	}

	rdr3::crypto::CTfit2CbcCipher cipher = rdr3::crypto::RDR2_KEYS.GetCipher(header.mDecryptionTag);
	cipher.Update((uint8_t*)entries.data(), (uint8_t*)entries.data(), sizeof(CRPF8Entry) * header.mEntryCount);

	std::shared_ptr<rdr3::fs::CFSDevice> device = std::make_shared<rdr3::fs::CFSDevice>(stream);

	for (CRPF8Entry& rpfEntry : entries) {
		if (rpfEntry.IsDirectory()) {
			continue;
		}

		size_t hash = rpfEntry.GetHash();
		device->AddFileEntry(rpfEntry);
	}

	return device;
}
