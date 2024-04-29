#pragma once

#include "types.h"

#include <filesystem>

namespace rdr3 {
	namespace fs {
        struct OodleLZDecoder;

        // The number of raw (uncompressed) bytes in each block (apart the final block, which may be smaller)
        constexpr uint32_t OODLELZ_BLOCK_LEN = (1 << 18);

        // Each block starts with a 2 byte header specifying the compression type.
        // If the compression type is memcpy, then the body of the block is just the uncomprsesed data (of length OODLELZ_BLOCK_LEN)
        // Therefore, the maximum compressed length should be (OODLELZ_BLOCK_LEN + 2).
        // However, it is unclear if this is actually the case, so just add a bit more.
        constexpr uint32_t OODLELZ_BLOCK_MAX_COMPLEN = (OODLELZ_BLOCK_LEN + 128);

        enum class OodleLZ_Compressor : int32_t
        {
            OodleLZ_Compressor_Invalid = -1,

            OodleLZ_Compressor_LZH,
            OodleLZ_Compressor_LZHLW,
            OodleLZ_Compressor_LZNIB,
            OodleLZ_Compressor_None,
            OodleLZ_Compressor_LZB16,
            OodleLZ_Compressor_LZBLW,
            OodleLZ_Compressor_LZA,
            OodleLZ_Compressor_LZNA,
            OodleLZ_Compressor_Kraken,
            OodleLZ_Compressor_Mermaid,
            OodleLZ_Compressor_BitKnit,
            OodleLZ_Compressor_Selkie,
            OodleLZ_Compressor_Hydra,
            OodleLZ_Compressor_Leviathan,
        };

		bool InitOodle(std::filesystem::path gameRoot);

		class COodleDecompressor {
        public:
            COodleDecompressor(int64_t size, OodleLZ_Compressor compressor = OodleLZ_Compressor::OodleLZ_Compressor_Invalid);
            ~COodleDecompressor();

            bool Reset();
            bool Update();

            size_t GetOptimalBufferSize();

            // Pointer and length of data waiting to be transformed
            const uint8_t* NextIn { };
            size_t AvailIn { };

            // Pointer and length of buffer to store transformed data
            uint8_t* NextOut { };
            size_t AvailOut { };

            // Indicates whether to expect any more input
            bool FinishedIn { };

            // Indicates whether to expect any more output
            bool FinishedOut { };

        private:
            int32_t FlushOutput();
            int32_t ConsumeInput();
            int32_t DecodeSome();

            // The expected total size of the decompressed data
            size_t mRealSize { };

            // Raw memory allocated for the decoder
            std::unique_ptr<uint8_t[]> mState { };

            // The actual decoder
            OodleLZDecoder* mDecoder { };

            // Total number of bytes decompressed
            size_t mTotalOut { };

            // The length of input data needed for the next block
            size_t mNeededIn { };

            // The length of data in buffer_in_
            size_t mBufferedIn { };

            // The range of pending data in buffer_out_
            size_t mOutputStart { };
            size_t mOutputEnd { };

            // Used to buffer the compressed data for a block
            std::unique_ptr<uint8_t[]> mBufferIn;

            // Used to buffer the decompresssed data from a block
            // Also acts as sliding window
            std::unique_ptr<uint8_t[]> mBufferOut;
		};
	}
}
