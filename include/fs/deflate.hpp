// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/asset/transform/deflate.cpp

#pragma once

#include "types.h"

#include <zlib-ng.h>

namespace rdr3 {
	namespace fs {
		class CDeflateDecompressor {
			zng_stream mInflater{ };

		public:
			CDeflateDecompressor(int32_t windowBits);
			~CDeflateDecompressor();

			bool Reset();
			bool Update(uint8_t* outData, uint8_t* inData, size_t outSize, size_t inSize);
		};
	}
}
