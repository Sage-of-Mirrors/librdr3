#pragma once

#include "types.h"
#include "keys.hpp"

#include <filesystem>

namespace rdr3 {
	namespace crypto {
		bool InitCrypto(std::filesystem::path gameRoot);

		extern CKeys RDR2_KEYS;
	} // namespace crypto
} // namespace rdr3
