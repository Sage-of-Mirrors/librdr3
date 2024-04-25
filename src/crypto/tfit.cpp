// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/crypto/tfit.cpp

#include "crypto/tfit.hpp"

constexpr size_t TFIT_BLOCK_SIZE = 16;

static void TFIT_DecryptRoundA(uint8_t data[16], const uint32_t key[4], const uint32_t table[16][256]) {
    uint32_t temp[4] {
        table[0][data[0]]   ^ table[1][data[1]]   ^ table[2][data[2]]   ^ table[3][data[3]]   ^ key[0],
        table[4][data[4]]   ^ table[5][data[5]]   ^ table[6][data[6]]   ^ table[7][data[7]]   ^ key[1],
        table[8][data[8]]   ^ table[9][data[9]]   ^ table[10][data[10]] ^ table[11][data[11]] ^ key[2],
        table[12][data[12]] ^ table[13][data[13]] ^ table[14][data[14]] ^ table[15][data[15]] ^ key[3],
    };

    std::memcpy(data, temp, 16);
}

static void TFIT_DecryptRoundB(uint8_t data[16], const uint32_t key[4], const uint32_t table[16][256]) {
    uint32_t temp[4] {
        table[0][data[0]]  ^ table[1][data[7]]  ^ table[2][data[10]] ^ table[3][data[13]]  ^ key[0],
        table[4][data[1]]  ^ table[5][data[4]]  ^ table[6][data[11]] ^ table[7][data[14]]  ^ key[1],
        table[8][data[2]]  ^ table[9][data[5]]  ^ table[10][data[8]] ^ table[11][data[15]] ^ key[2],
        table[12][data[3]] ^ table[13][data[6]] ^ table[14][data[9]] ^ table[15][data[12]] ^ key[3],
    };

    std::memcpy(data, temp, 16);
}

static void TFIT_DecryptBlock(const uint8_t input[16], uint8_t output[16], const uint32_t keys[17][4], const uint32_t tables[17][16][256]) {
    uint8_t temp[16];
    std::memcpy(temp, input, 16);

    TFIT_DecryptRoundA(temp, keys[0], tables[0]);
    TFIT_DecryptRoundA(temp, keys[1], tables[1]);

    for (size_t i = 2; i < 16; ++i) {
        TFIT_DecryptRoundB(temp, keys[i], tables[i]);
    }

    TFIT_DecryptRoundA(temp, keys[16], tables[16]);

    std::memcpy(output, temp, 16);
}

CTfitEcbCipher::CTfitEcbCipher(const uint32_t keys[17][4], const uint32_t tables[17][16][256]) : keys_(keys), tables_(tables) {

}

size_t CTfitEcbCipher::Update(const uint8_t* input, uint8_t* output, size_t length) {
    for (size_t blocks = length / TFIT_BLOCK_SIZE; blocks; --blocks, input += TFIT_BLOCK_SIZE, output += TFIT_BLOCK_SIZE) {
        TFIT_DecryptBlock(input, output, keys_, tables_);
    }

    return length;
}

size_t CTfitEcbCipher::GetBlockSize() {
    return TFIT_BLOCK_SIZE;
}

CTfitCbcCipher::CTfitCbcCipher(const uint32_t keys[17][4], const uint32_t tables[17][16][256], const uint8_t iv[16]) : keys_(keys), tables_(tables) {
    std::memcpy(iv_, iv, 0x10);
}

size_t CTfitCbcCipher::Update(const uint8_t* input, uint8_t* output, size_t length) {
    uint8_t iv[2][16];

    uint8_t* current_iv = iv_;

    for (size_t blocks = length / TFIT_BLOCK_SIZE; blocks; --blocks, input += TFIT_BLOCK_SIZE, output += TFIT_BLOCK_SIZE) {
        uint8_t* const next_iv = iv[blocks & 1];

        std::memcpy(next_iv, input, TFIT_BLOCK_SIZE);

        TFIT_DecryptBlock(input, output, keys_, tables_);

        for (size_t j = 0; j < 16; ++j) {
            output[j] ^= current_iv[j];
        }

        current_iv = next_iv;
    }

    std::memcpy(iv_, current_iv, 16);

    return length;
}

size_t CTfitCbcCipher::GetBlockSize() {
    return TFIT_BLOCK_SIZE;
}
