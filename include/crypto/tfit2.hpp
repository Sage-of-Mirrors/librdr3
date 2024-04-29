// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/crypto/tfit2.h

#pragma once

#include "types.h"
#include "cipher.hpp"

namespace rdr3 {
    namespace crypto {
        struct CTfit2Context {
            uint64_t InitTables[16][256];

            struct Round
            {
                uint64_t Lookup[4096];

                struct Block {
                    uint64_t Masks[16];
                    uint32_t Xor;
                } Blocks[16];
            } Rounds[17];

            uint64_t EndMasks[16][8];
            uint8_t EndTables[16][256];
            uint8_t EndXor[16];
        };

        struct CTfit2Key {
            uint64_t Data[18][2];
        };

        class CTfit2CbcCipher final : public CCipher {
        public:
            CTfit2CbcCipher(const CTfit2Key* key, const uint8_t iv[16], const CTfit2Context* ctx);

            size_t Update(const uint8_t* input, uint8_t* output, size_t length) override;

            size_t GetBlockSize() override;

        private:
            const uint64_t(*keys_)[2] {};
            const CTfit2Context* ctx_{};
            uint8_t iv_[16]{};
        };
    }
}
