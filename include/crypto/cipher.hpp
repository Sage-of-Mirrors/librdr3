// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/crypto/cipher.h

#pragma once

#include "types.h"

namespace rdr3 {
    namespace crypto {
        class CCipher {
        public:
            virtual ~CCipher() = default;

            // Processing length bytes from input and writes them to output
            // input may equal output, otherwise input must not overlap with output
            virtual size_t Update(const uint8_t* input, uint8_t* output, size_t length) = 0;

            virtual size_t GetBlockSize() = 0;

            inline size_t Update(void* data, size_t length) {
                return Update(static_cast<const uint8_t*>(data), static_cast<uint8_t*>(data), length);
            }
        };
    }
}
