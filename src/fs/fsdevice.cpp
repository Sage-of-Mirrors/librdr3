#include "fs/fsdevice.hpp"
#include "fs/rpf8.hpp"
#include "fs/deflate.hpp"
#include "fs/oodle.hpp"

#include "crypto/crypto.hpp"
#include "crypto/cipher.hpp"
#include "crypto/keys.hpp"
#include "crypto/stridedcipher.hpp"

#include "util/bstream.h"
#include "util/util.hpp"

rdr3::fs::CFSDevice::CFSDevice(std::unique_ptr<bStream::CStream>& stream) {
	mDataStream = std::move(stream);
}

void rdr3::fs::CFSDevice::AddFileEntry(CRPF8Entry entry) {
	mFileEntries.emplace(entry.GetHash(), entry);
}

void rdr3::fs::CFSDevice::ExpandFile(uint8_t* expandedFile, uint8_t* diskFile, const CRPF8Entry* entry) {
	bool bIsCompressed = entry->GetCompressorId() != ECompressionType::CMP_NONE;
	uint32_t expandedFileSize = entry->GetSize();
	uint32_t diskFileSize = entry->GetOnDiskSize();
	
	// Decryption

	int64_t chunkSize = entry->IsResource()
		? (bIsCompressed ? 0x80000 : entry->GetSize())
		: (bIsCompressed ? 0x2000  : 0x1000);

	rdr3::crypto::CTfit2CbcCipher cipher = rdr3::crypto::RDR2_KEYS.GetCipher(entry->GetEncryptionKeyId());
	rdr3::crypto::CStridedCipher stridedCipher(entry->GetEncryptionConfig(), diskFileSize, &cipher, chunkSize);
	stridedCipher.Update(diskFile, diskFile, diskFileSize);

	// Decompression

	if (bIsCompressed) {
		switch (entry->GetCompressorId()) {
			case ECompressionType::CMP_DEFLATE:
			{
				CDeflateDecompressor deflateDecomp(-15);
				deflateDecomp.Update(expandedFile, diskFile, expandedFileSize, diskFileSize);
				break;
			}
			case ECompressionType::CMP_OODLE:
			{
				COodleDecompressor oodleDecomp(expandedFileSize);
				oodleDecomp.NextIn = diskFile;
				oodleDecomp.AvailIn = diskFileSize;

				oodleDecomp.NextOut = expandedFile;
				oodleDecomp.AvailOut = expandedFileSize;

				oodleDecomp.FinishedIn = true;

				oodleDecomp.Update();
				break;
			}
			default:
			{
				std::cout << "Unknown compression type found, how did we get here?" << std::endl;
				break;
			}
		}
	}
	else {
		std::memcpy(expandedFile, diskFile, expandedFileSize);
	}
}

uint8_t* rdr3::fs::CFSDevice::GetFile(std::filesystem::path path, uint32_t& size) {
	return GetFile(JenkinsHashString(path.u8string()), size);
}

uint8_t* rdr3::fs::CFSDevice::GetFile(uint32_t hash, uint32_t& size) {
	auto iter = mFileEntries.find(hash);
	if (iter == mFileEntries.end()) {
		return nullptr;
	}

	const CRPF8Entry& entry = iter->second;

	size = entry.GetSize();
	uint64_t diskFileSize = entry.GetOnDiskSize();
	uint64_t diskOffset = entry.GetOffset();

	if (entry.IsSignatureProtected()) {
		diskFileSize -= 0x100;
	}
	if (entry.IsResource()) {
		diskOffset += 0x10;
		diskFileSize -= 0x10;
	}

	uint8_t* diskFile = new uint8_t[diskFileSize];
	uint8_t* expandedFile = new uint8_t[size];

	mDataStream->seek(diskOffset);
	mDataStream->readBytesTo(diskFile, diskFileSize);

	ExpandFile(expandedFile, diskFile, &entry);

	delete[] diskFile;
	return expandedFile;
}
