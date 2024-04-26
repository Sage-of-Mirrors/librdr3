#pragma once

#include "types.h"

namespace rdr3 {
	namespace crypto {
		class CKeys {
			uint8_t mAESKey[32];

		public:
			CKeys();
			
			bool TryLoad();
			void SetAESKey(const uint8_t* key);
		};
	} // namespace crypto
} // namespace rdr3
