// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/crypto/tfit2.cpp

#include "crypto/tfit2.hpp"

constexpr size_t TFIT2_BLOCK_SIZE = 16;

static void TFIT2_DecryptSplatBytes(uint8_t bytes[8], uint64_t output[8]) {
    for (size_t i = 0; i < 8; ++i) {
        output[i] = 0x0101010101010101u * bytes[7 - i];
    }
}

static uint64_t TFIT2_DecryptRoundBlock(const uint64_t input1[8], const uint64_t masks[16], uint32_t xorr, const uint64_t lookup[4096]) {
    // 4096 = 256 * 16
    uint64_t lower = masks[0xF] & input1[0x7] ^ masks[0xE] & input1[0x6] ^ masks[0xD] & input1[0x5] ^
        masks[0xC] & input1[0x4] ^ masks[0xB] & input1[0x3] ^ masks[0xA] & input1[0x2] ^ masks[0x9] & input1[0x1] ^
        masks[0x8] & input1[0x0];

    lower ^= lower >> 1;
    lower ^= lower >> 2;
    lower ^= lower >> 4;
    lower &= 0x101010101010101u;

    lower |= lower >> 7;
    lower |= lower >> 14;
    lower |= lower >> 28;

    uint64_t upper = masks[0x7] & input1[0x7] ^ masks[0x6] & input1[0x6] ^ masks[0x5] & input1[0x5] ^
        masks[0x4] & input1[0x4] ^ masks[0x3] & input1[0x3] ^ masks[0x2] & input1[0x2] ^ masks[0x1] & input1[0x1] ^
        masks[0x0] & input1[0x0];

    upper ^= upper << 1;
    upper ^= upper >> 2;
    upper ^= upper >> 4;
    upper &= 0x202020202020202u;

    upper |= upper << 7;
    upper |= upper >> 14;

    return lookup[(lower & 0xFF) ^ (upper & 0xF00) ^ xorr];
}

static void TFIT2_DecryptRoundA(const rdr3::crypto::CTfit2Context& ctx, uint8_t data[16]) {
    const uint64_t values[2] {
        ctx.InitTables[0x0][data[0x0]] ^ ctx.InitTables[0x1][data[0x1]] ^ ctx.InitTables[0x2][data[0x2]] ^
        ctx.InitTables[0x3][data[0x3]] ^ ctx.InitTables[0x4][data[0x4]] ^ ctx.InitTables[0x5][data[0x5]] ^
        ctx.InitTables[0x6][data[0x6]] ^ ctx.InitTables[0x7][data[0x7]],

        ctx.InitTables[0x8][data[0x8]] ^ ctx.InitTables[0x9][data[0x9]] ^ ctx.InitTables[0xA][data[0xA]] ^
        ctx.InitTables[0xB][data[0xB]] ^ ctx.InitTables[0xC][data[0xC]] ^ ctx.InitTables[0xD][data[0xD]] ^
        ctx.InitTables[0xE][data[0xE]] ^ ctx.InitTables[0xF][data[0xF]],
    };

    std::memcpy(data, values, 16);
}

static void TFIT2_DecryptRoundB(const rdr3::crypto::CTfit2Context& ctx, size_t index, uint8_t data[16], const uint64_t key[2]) {
    const auto& round = ctx.Rounds[index];

    uint64_t v0[8], v1[8];

    TFIT2_DecryptSplatBytes(data + 0, v0);
    TFIT2_DecryptSplatBytes(data + 8, v1);

    const uint64_t val[2] {
        key[0] ^ TFIT2_DecryptRoundBlock(v0, round.Blocks[0x0].Masks, round.Blocks[0x0].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x1].Masks, round.Blocks[0x1].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x2].Masks, round.Blocks[0x2].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x3].Masks, round.Blocks[0x3].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x4].Masks, round.Blocks[0x4].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x5].Masks, round.Blocks[0x5].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x6].Masks, round.Blocks[0x6].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x7].Masks, round.Blocks[0x7].Xor, round.Lookup),

        key[1] ^ TFIT2_DecryptRoundBlock(v1, round.Blocks[0x8].Masks, round.Blocks[0x8].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0x9].Masks, round.Blocks[0x9].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0xA].Masks, round.Blocks[0xA].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0xB].Masks, round.Blocks[0xB].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0xC].Masks, round.Blocks[0xC].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0xD].Masks, round.Blocks[0xD].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0xE].Masks, round.Blocks[0xE].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0xF].Masks, round.Blocks[0xF].Xor, round.Lookup),
    };

    std::memcpy(data, val, 16);
}

static void TFIT2_DecryptRoundC(const rdr3::crypto::CTfit2Context& ctx, size_t index, uint8_t data[16], const uint64_t key[2]) {
    const auto& round = ctx.Rounds[index];

    uint64_t v0[8], v1[8];

    TFIT2_DecryptSplatBytes(data + 0, v0);
    TFIT2_DecryptSplatBytes(data + 8, v1);

    const uint64_t val[2] {
        key[0] ^ TFIT2_DecryptRoundBlock(v0, round.Blocks[0x0].Masks, round.Blocks[0x0].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0x1].Masks, round.Blocks[0x1].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0x2].Masks, round.Blocks[0x2].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x3].Masks, round.Blocks[0x3].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x4].Masks, round.Blocks[0x4].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x5].Masks, round.Blocks[0x5].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0x6].Masks, round.Blocks[0x6].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0x7].Masks, round.Blocks[0x7].Xor, round.Lookup),

        key[1] ^ TFIT2_DecryptRoundBlock(v1, round.Blocks[0x8].Masks, round.Blocks[0x8].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0x9].Masks, round.Blocks[0x9].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0xA].Masks, round.Blocks[0xA].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0xB].Masks, round.Blocks[0xB].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0xC].Masks, round.Blocks[0xC].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v1, round.Blocks[0xD].Masks, round.Blocks[0xD].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0xE].Masks, round.Blocks[0xE].Xor, round.Lookup) ^
                 TFIT2_DecryptRoundBlock(v0, round.Blocks[0xF].Masks, round.Blocks[0xF].Xor, round.Lookup),
    };

    std::memcpy(data, val, 16);
}

static uint8_t TFIT2_DecryptSquashBytes(const uint64_t input[8], const uint64_t lookup[8]) {
    uint64_t v1 = lookup[7] & input[7] ^ lookup[6] & input[6] ^ lookup[5] & input[5] ^ lookup[4] & input[4] ^
        lookup[3] & input[3] ^ lookup[2] & input[2] ^ lookup[1] & input[1] ^ lookup[0] & input[0];

    v1 ^= v1 >> 1;
    v1 ^= v1 >> 2;
    v1 ^= v1 >> 4;

    v1 &= 0x101010101010101u; // Each byte is 1 bit, the "sum" of xoring all bits of that byte

    // Compress the value back into a byte (using the lowest bit of each byte)
    v1 |= v1 >> 7;
    v1 |= v1 >> 14;
    v1 |= v1 >> 28;

    return (uint8_t)(v1);
}

static void TFIT2_DecryptRoundD(const rdr3::crypto::CTfit2Context& ctx, uint8_t data[16]) {
    uint64_t v0[8], v1[8];

    TFIT2_DecryptSplatBytes(data + 0, v0);
    TFIT2_DecryptSplatBytes(data + 8, v1);

    data[0x0] = ctx.EndTables[0x0][TFIT2_DecryptSquashBytes(v0, ctx.EndMasks[0x0]) ^ ctx.EndXor[0x0]];
    data[0x1] = ctx.EndTables[0x1][TFIT2_DecryptSquashBytes(v0, ctx.EndMasks[0x1]) ^ ctx.EndXor[0x1]];
    data[0x2] = ctx.EndTables[0x2][TFIT2_DecryptSquashBytes(v0, ctx.EndMasks[0x2]) ^ ctx.EndXor[0x2]];
    data[0x3] = ctx.EndTables[0x3][TFIT2_DecryptSquashBytes(v0, ctx.EndMasks[0x3]) ^ ctx.EndXor[0x3]];
    data[0x4] = ctx.EndTables[0x4][TFIT2_DecryptSquashBytes(v0, ctx.EndMasks[0x4]) ^ ctx.EndXor[0x4]];
    data[0x5] = ctx.EndTables[0x5][TFIT2_DecryptSquashBytes(v0, ctx.EndMasks[0x5]) ^ ctx.EndXor[0x5]];
    data[0x6] = ctx.EndTables[0x6][TFIT2_DecryptSquashBytes(v0, ctx.EndMasks[0x6]) ^ ctx.EndXor[0x6]];
    data[0x7] = ctx.EndTables[0x7][TFIT2_DecryptSquashBytes(v0, ctx.EndMasks[0x7]) ^ ctx.EndXor[0x7]];

    data[0x8] = ctx.EndTables[0x8][TFIT2_DecryptSquashBytes(v1, ctx.EndMasks[0x8]) ^ ctx.EndXor[0x8]];
    data[0x9] = ctx.EndTables[0x9][TFIT2_DecryptSquashBytes(v1, ctx.EndMasks[0x9]) ^ ctx.EndXor[0x9]];
    data[0xA] = ctx.EndTables[0xA][TFIT2_DecryptSquashBytes(v1, ctx.EndMasks[0xA]) ^ ctx.EndXor[0xA]];
    data[0xB] = ctx.EndTables[0xB][TFIT2_DecryptSquashBytes(v1, ctx.EndMasks[0xB]) ^ ctx.EndXor[0xB]];
    data[0xC] = ctx.EndTables[0xC][TFIT2_DecryptSquashBytes(v1, ctx.EndMasks[0xC]) ^ ctx.EndXor[0xC]];
    data[0xD] = ctx.EndTables[0xD][TFIT2_DecryptSquashBytes(v1, ctx.EndMasks[0xD]) ^ ctx.EndXor[0xD]];
    data[0xE] = ctx.EndTables[0xE][TFIT2_DecryptSquashBytes(v1, ctx.EndMasks[0xE]) ^ ctx.EndXor[0xE]];
    data[0xF] = ctx.EndTables[0xF][TFIT2_DecryptSquashBytes(v1, ctx.EndMasks[0xF]) ^ ctx.EndXor[0xF]];
}

static void TFIT2_DecryptBlock(const rdr3::crypto::CTfit2Context& ctx, const uint64_t key[17][2], const uint8_t input[16], uint8_t output[16]) {
    uint8_t temp[16];
    std::memcpy(temp, input, 16);

    TFIT2_DecryptRoundA(ctx, temp);

    TFIT2_DecryptRoundB(ctx, 0, temp, key[0]);
    TFIT2_DecryptRoundB(ctx, 1, temp, key[1]);

    for (size_t i = 2; i < 16; ++i) {
        TFIT2_DecryptRoundC(ctx, i, temp, key[i]);
    }

    TFIT2_DecryptRoundB(ctx, 16, temp, key[16]);

    TFIT2_DecryptRoundD(ctx, temp);

    std::memcpy(output, temp, 16);
}

rdr3::crypto::CTfit2CbcCipher::CTfit2CbcCipher(const CTfit2Key* key, const uint8_t iv[16], const CTfit2Context* ctx) : keys_(key->Data + 1), ctx_(ctx) {
    std::memcpy(iv_, iv, 16);
}

size_t rdr3::crypto::CTfit2CbcCipher::Update(const uint8_t* input, uint8_t* output, size_t length) {
    uint8_t iv[2][16];

    uint8_t* current_iv = iv_;

    for (size_t blocks = length / TFIT2_BLOCK_SIZE; blocks; --blocks, input += TFIT2_BLOCK_SIZE, output += TFIT2_BLOCK_SIZE) {
        uint8_t* const next_iv = iv[blocks & 1];

        std::memcpy(next_iv, input, TFIT2_BLOCK_SIZE);

        TFIT2_DecryptBlock(*ctx_, keys_, input, output);

        for (size_t j = 0; j < 16; ++j) {
            output[j] ^= current_iv[j];
        }

        current_iv = next_iv;
    }

    std::memcpy(iv_, current_iv, 16);

    return length;
}

size_t rdr3::crypto::CTfit2CbcCipher::GetBlockSize() {
    return TFIT2_BLOCK_SIZE;
}
