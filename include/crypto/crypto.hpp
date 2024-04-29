#pragma once

#include "types.h"

#include <filesystem>

namespace rdr3 {
	namespace crypto {
		extern class CKeys RDR2_KEYS;

		bool InitCrypto(std::filesystem::path gameRoot);
	} // namespace crypto
} // namespace rdr3
