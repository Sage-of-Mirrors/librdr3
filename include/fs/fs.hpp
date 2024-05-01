#pragma once

#include "types.h"

#include <filesystem>

namespace rdr3 {
	namespace fs {
		bool InitFS(std::filesystem::path gameRoot);

		uint8_t* GetFileData(std::filesystem::path filePath, uint32_t& fileSize);
	}
}
