#pragma once

#include "types.h"
#include "crypto/tfit2.hpp"

#include <unordered_map>

namespace rdr3 {
	namespace crypto {
		class CSecretStore;

		class CKeys {
			CTfit2Context mContext;
			std::unordered_map<size_t, CTfit2Key> mKeys;
			uint8_t mInitVector[16];

		public:
			CKeys();

			bool LoadKeys(CSecretStore* secrets);
			CTfit2CbcCipher GetCipher(uint16_t tag);
		};
	} // namespace crypto
} // namespace rdr3
