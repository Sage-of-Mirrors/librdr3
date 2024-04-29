// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/games/rage/rpf8.cpp#L106

#pragma once

#include "types.h"
#include "cipher.hpp"

namespace rdr3 {
    namespace crypto {
        class CStridedCipher : public CCipher
        {
        public:
            uint64_t Offset { };
            CCipher* mCipher;
            std::vector<std::pair<uint64_t, uint64_t>> Blocks;

            CStridedCipher(uint8_t config, int64_t size, CCipher* cipher, int64_t chunk_size);

            size_t Update(const uint8_t* input, uint8_t* output, size_t length) override;
            size_t GetBlockSize() override;

        private:
            void UpdateBlocks(const uint8_t* input, uint8_t* output, size_t length);
        };
    }
}