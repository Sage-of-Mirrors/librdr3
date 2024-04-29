// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/games/rage/rpf8.cpp#L106

#include "crypto/stridedcipher.hpp"

namespace {
    // rage::AES::UnpackConfig
// head_length  in [0, 0x1000, 0x4000, 0x10000]
// block_length in [0, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000]
// block_stride in [0, 0x10000, 0x20000, 0x30000, 0x40000, 0x50000, 0x60000, 0x70000, 0x80000]
    static void UnpackConfig(uint8_t config, int64_t& head_length, int64_t& block_length, int64_t& block_stride)
    {
        if (uint8_t head_config = config & 0b11) // Config[0:2]
        {
            head_length = int64_t(0x400) << (head_config * 2);
        }

        if (uint8_t length_config = (config >> 2) & 0b111) // Config[2:5]
        {
            uint8_t stride_config = (config >> 5) & 0b111; // Config[5:8]
            block_length = int64_t(0x400) << length_config;
            block_stride = int64_t(stride_config + 1) << 16;
        }
    }

    static std::vector<std::pair<uint64_t, uint64_t>> UnpackConfig(uint8_t config, int64_t file_size, int64_t chunk_size)
    {
        std::vector<std::pair<uint64_t, uint64_t>> results;
        int64_t offset = 0;

        const auto Update = [&](int64_t start, int64_t end) {
            start = std::max<int64_t>(start, offset);
            end = std::min<int64_t>(end, file_size);

            if (start < end)
            {
                results.push_back({ static_cast<uint64_t>(start), static_cast<uint64_t>(end) });
                offset = end;
            }
            };

        int64_t tail_length = 0x400;
        int64_t tail_offset = file_size - tail_length;

        int64_t head_length = 0;
        int64_t block_length = 0;
        int64_t block_stride = 0;

        UnpackConfig(config, head_length, block_length, block_stride);

        Update(0, head_length);

        if (head_length < tail_offset)
        {
            if (block_length || block_stride)
            {
                if ((block_stride == chunk_size) && (tail_offset < block_stride) && (block_stride < file_size))
                {
                    // There is a bug in rage::AES::Cipher.
                    // Under certain conditions, the ranges of data it ciphers can change depending on how you call it.
                    // SwLogInfo("{:02X} {:X}", config, file_size);

                    // This should mirror what the game does, but a few files still fail to decompress
                    // (and some others probably just decompress incorrectly, as there are no CRCs)
                    offset = block_stride;
                }
                else
                {
                    for (int64_t block = offset ? block_stride : 0; block + block_length <= tail_offset;
                        block += block_stride)
                    {
                        Update(block, block + block_length);
                    }
                }
            }

            Update(tail_offset, file_size);
        }

        return results;
    }
}

rdr3::crypto::CStridedCipher::CStridedCipher(uint8_t config, int64_t size, CCipher* cipher, int64_t chunk_size)
    : mCipher(cipher), Blocks(UnpackConfig(config, size, chunk_size)) {

}

size_t rdr3::crypto::CStridedCipher::Update(const uint8_t* input, uint8_t* output, size_t length) {
    UpdateBlocks(input, output, length);
    return length;
}

size_t rdr3::crypto::CStridedCipher::GetBlockSize()
{
    return 16;
}

void rdr3::crypto::CStridedCipher::UpdateBlocks(const uint8_t* input, uint8_t* output, size_t length)
{
    const auto update = [&](uint64_t offset, bool cipher) {
        if (size_t n = static_cast<size_t>(std::min<uint64_t>(offset - Offset, length)))
        {
            if (cipher)
                mCipher->Update(input, output, n);
            else if (input != output)
                std::memcpy(output, input, n);

            Offset += n;
            input += n;
            output += n;
            length -= n;
        }

        return Offset == offset;
        };

    for (const auto& [start, end] : Blocks)
    {
        if (Offset >= end)
            continue;

        if (Offset < start)
        {
            if (!update(start, false))
                return;
        }

        if (!update(end, true))
            return;
    }

    update(UINT64_MAX, false);
}
