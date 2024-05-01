#pragma once

#include "types.h"

#include <unordered_map>
#include <filesystem>

struct CRPF8Entry;

namespace rdr3 {
	namespace fs {
		struct CFSFileEntry {
			size_t mSizeOnDisk;
			size_t mSize;
			size_t mOffset;
		};

		class CFSDevice {
			std::unique_ptr<bStream::CStream> mDataStream;
			std::unordered_map<size_t, CRPF8Entry> mFileEntries;

			void ExpandFile(uint8_t* expandedFile, uint8_t* diskFile, const CRPF8Entry* entry);

		public:
			CFSDevice(std::unique_ptr<bStream::CStream>& stream);

			void AddFileEntry(CRPF8Entry entry);

			uint8_t* GetFileData(std::filesystem::path path, uint32_t& size);
			uint8_t* GetFileData(uint32_t hash, uint32_t& size);
		};
	}
}
