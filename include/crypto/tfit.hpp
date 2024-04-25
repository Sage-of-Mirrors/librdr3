// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/crypto/tfit.h

#pragma once

#include "types.h"
#include "cipher.hpp"

class CTfitEcbCipher final : public CCipher {
public:
    CTfitEcbCipher(const uint32_t keys[17][4], const uint32_t tables[17][16][256]);

    size_t Update(const uint8_t* input, uint8_t* output, size_t length) override;

    size_t GetBlockSize() override;

    const uint32_t(*keys_)[4] { };
    const uint32_t(*tables_)[16][256] { };
};

class CTfitCbcCipher final : public CCipher {
public:
    CTfitCbcCipher(const uint32_t keys[17][4], const uint32_t tables[17][16][256], const uint8_t iv[16]);

    size_t Update(const uint8_t* input, uint8_t* output, size_t length) override;

    size_t GetBlockSize() override;

    const uint32_t(*keys_)[4] {};
    const uint32_t(*tables_)[16][256]{};

    uint8_t iv_[16]{};
};
