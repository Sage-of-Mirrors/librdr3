#include "fs/oodle.hpp"

#include <Windows.h>
#include <libloaderapi.h>

constexpr char OODLE_DLL_NAME[] = "oo2core_5_win64.dll";

namespace {
	HMODULE OODLE_DLL;

	const char* OodleProcNames[] {
		"OodleLZDecoder_MemorySizeNeeded",
		"OodleLZDecoder_Create",
		"OodleLZDecoder_Reset",
		"OodleLZDecoder_Destroy",
		"OodleLZDecoder_DecodeSome"
	};

	using OodleProc = void (*)();

    #define OODLELZ_FAILED 0

    #if defined(_WIN32)
    #    define OODLEAPI __stdcall
    #else
    #    define OODLEAPI
    #endif

    struct OodleLZDecoder;

    enum OodleLZ_Verbosity : int32_t
    {
        OodleLZ_Verbosity_None,
        OodleLZ_Verbosity_Max = 3, // Unknown Name
    };

    enum OodleLZ_FuzzSafe : int32_t
    {
        OodleLZ_FuzzSafe_No,
        OodleLZ_FuzzSafe_Yes,
    };

    enum OodleLZ_CheckCRC : int32_t
    {
        OodleLZ_CheckCRC_No,
        OodleLZ_CheckCRC_Yes,
    };

    enum OodleLZ_Decode_ThreadPhase : int32_t
    {
        OodleLZ_Decode_ThreadPhase1 = 0x1,
        OodleLZ_Decode_ThreadPhase2 = 0x2,

        OodleLZ_Decode_Unthreaded = OodleLZ_Decode_ThreadPhase1 | OodleLZ_Decode_ThreadPhase2,
    };

    // Used to indicate the progress of OodleLZDecoder_DecodeSome
    // If OodleLZDecoder_DecodeSome returns true, it should always contain valid/zeroed data
    struct OodleLZ_DecodeSome_Out
    {
        // Number of uncompressed bytes decoded
        // This will only be non-zero if the whole block/quantum was decoded
        uint32_t decodedCount;

        // Number of compressed bytes consumed
        // If only the block header is consumed, this will be zero
        // If the quantum header is consumed
        // This may be non-zero while decodedCount is zero, if only the quantum head
        uint32_t compBufUsed;

        // You must have at least this much room available in the output buffer
        // Should always be <= OODLELZ_BLOCK_LEN
        // Ignore if decodedCount != 0
        uint32_t curQuantumRawLen;

        // Amount of input data needed to decode the current quantum.
        // Should always be <= OODLELZ_BLOCK_MAX_COMPLEN
        // Ignore if decodedCount != 0
        uint32_t curQuantumCompLen;
    };

    static union
    {
        struct
        {
            int32_t(OODLEAPI* LZDecoder_MemorySizeNeeded)(rdr3::fs::OodleLZ_Compressor compressor, size_t rawLen);

            rdr3::fs::OodleLZDecoder* (OODLEAPI* LZDecoder_Create)(
                rdr3::fs::OodleLZ_Compressor compressor, int64_t rawLen, void* memory, size_t memorySize);

            int32_t(OODLEAPI* LZDecoder_Reset)(rdr3::fs::OodleLZDecoder* decoder, size_t decPos, size_t decLen);

            void(OODLEAPI* LZDecoder_Destroy)(rdr3::fs::OodleLZDecoder* decoder);

            int32_t(OODLEAPI* LZDecoder_DecodeSome)(rdr3::fs::OodleLZDecoder* decoder, OodleLZ_DecodeSome_Out* out, void* decBuf,
                size_t decBufPos, size_t decBufferSize, size_t decBufAvail, const void* compPtr, size_t compAvail,
                OodleLZ_FuzzSafe fuzzSafe, OodleLZ_CheckCRC checkCRC, OodleLZ_Verbosity verbosity,
                OodleLZ_Decode_ThreadPhase threadPhase);
        } Oodle;

        OodleProc OodleProcs[5];

        static_assert(sizeof(Oodle) == sizeof(OodleProcs));
    };
}

bool rdr3::fs::InitOodle(std::filesystem::path gameRoot) {
	std::filesystem::path appOodlePath = std::filesystem::current_path() / OODLE_DLL_NAME;
	std::filesystem::path gameOodlePath = gameRoot / OODLE_DLL_NAME;

	if (!std::filesystem::exists(appOodlePath)) {
		if (!std::filesystem::exists(gameOodlePath)) {
			return false;
		}

		std::filesystem::copy_file(gameOodlePath, appOodlePath);
	}

	OODLE_DLL = LoadLibraryW(gameOodlePath.c_str());
	if (OODLE_DLL == NULL || OODLE_DLL == INVALID_HANDLE_VALUE) {
		return false;
	}

	for (uint32_t i = 0; i < 5; i++) {
        OodleProcs[i] = reinterpret_cast<OodleProc>(GetProcAddress(OODLE_DLL, OodleProcNames[i]));

		if (OodleProcs[i] == NULL) {
			return false;
		}
	}

    return true;
}

/* COodleDecompressor */

rdr3::fs::COodleDecompressor::COodleDecompressor(int64_t size, OodleLZ_Compressor compressor) : mRealSize(size) {
    size_t length = Oodle.LZDecoder_MemorySizeNeeded(compressor, -1);
    mState = std::make_unique<uint8_t[]>(length);

    mDecoder = Oodle.LZDecoder_Create(compressor, mRealSize, mState.get(), length);

    mBufferIn = std::make_unique<uint8_t[]>(OODLELZ_BLOCK_MAX_COMPLEN);
    mBufferOut = std::make_unique<uint8_t[]>(OODLELZ_BLOCK_LEN);
}

rdr3::fs::COodleDecompressor::~COodleDecompressor() {
    Oodle.LZDecoder_Destroy(mDecoder);
}

bool rdr3::fs::COodleDecompressor::Reset() {
    mTotalOut = 0;

    mNeededIn = 0;
    mBufferedIn = 0;

    mOutputStart = 0;
    mOutputEnd = 0;

    FinishedOut = false;

    return Oodle.LZDecoder_Reset(mDecoder, 0, 0);
}

bool rdr3::fs::COodleDecompressor::Update() {
    while (true)
    {
        // Try and flush any pending output
        if (int32_t error = FlushOutput())
            return error > 0;

        // Ensure we have enough input
        if (int32_t error = ConsumeInput())
            return error > 0;

        // Try and decode the input
        if (int32_t error = DecodeSome())
            return error > 0;
    }
}

size_t rdr3::fs::COodleDecompressor::GetOptimalBufferSize() {
	return OODLELZ_BLOCK_LEN;
}

int32_t rdr3::fs::COodleDecompressor::FlushOutput() {
    // Check if we have any buffered data to flush
    if (size_t buffered = mOutputEnd - mOutputStart)
    {
        // See how much of the buffered data we can write
        size_t avail = std::min<size_t>(buffered, AvailOut);

        // If we can write any output, do it
        if (avail)
        {
            std::memcpy(NextOut, &mBufferOut[mOutputStart], avail);
            NextOut += avail;
            AvailOut -= avail;

            mOutputStart += avail;
            mTotalOut += avail;
            buffered -= avail;
        }

        // There is still data to write, so return
        if (buffered)
            return 1;

        // Time to start a new block
        if (mOutputEnd == OODLELZ_BLOCK_LEN)
        {
            mOutputStart = 0;
            mOutputEnd = 0;
        }
    }

    // Check if we've decompressed the whole thing
    if (mTotalOut >= mRealSize)
    {
        // We've already reached the end once, so now report it as an error
        if (FinishedOut)
            return -1;

        FinishedOut = true;
        return 1;
    }

    return 0;
}

int32_t rdr3::fs::COodleDecompressor::ConsumeInput() {
    // Check if we need more input
    if (size_t needed = mNeededIn - mBufferedIn)
    {
        // See how much of that input we can read
        size_t avail = std::min<size_t>(needed, AvailIn);

        // If we can read any, do it
        if (avail)
        {
            std::memcpy(&mBufferIn[mBufferedIn], NextIn, avail);
            NextIn += avail;
            AvailIn -= avail;

            mBufferedIn += avail;
            needed -= avail;
        }

        if (FinishedIn)
        {
            // If there is no more input, try and process what we have
            if (mBufferedIn)
                mNeededIn = mBufferedIn;
            else if (avail == 0)
                return -1;
        }
        else if (needed)
        {
            return 1;
        }
    }

    return 0;
}

int32_t rdr3::fs::COodleDecompressor::DecodeSome() {
    // TODO: When possible, decode directly to NextOut

    // Older versions of Oodle would report an error when given 0 bytes of input
    if (mBufferedIn == 0)
    {
        mNeededIn = 0x20;

        return 0;
    }

    OodleLZ_DecodeSome_Out out{};

    if (Oodle.LZDecoder_DecodeSome(mDecoder, &out, mBufferOut.get(), mOutputEnd, mRealSize,
        OODLELZ_BLOCK_LEN - mOutputEnd, mBufferIn.get(), mBufferedIn, OodleLZ_FuzzSafe_No, OodleLZ_CheckCRC_Yes,
        OodleLZ_Verbosity_None, OodleLZ_Decode_Unthreaded) <= 0)
        return -1;

    // Check if the any input was consumed
    if (out.compBufUsed)
    {
        // Check if there is any extra data in the input buffer
        size_t extra = mBufferedIn - out.compBufUsed;

        if (extra)
        {
            // This should never be more than a few bytes
            std::memmove(mBufferIn.get(), &mBufferIn[out.compBufUsed], extra);
        }

        mBufferedIn = extra;
    }

    mNeededIn = mBufferedIn;

    if (out.decodedCount)
    {
        mOutputEnd += out.decodedCount;
    }
    else
    {
        if (out.curQuantumRawLen)
        {
            if (mOutputEnd + out.curQuantumRawLen > OODLELZ_BLOCK_LEN)
                return -1;
        }

        if (out.curQuantumCompLen)
        {
            if (out.curQuantumCompLen > OODLELZ_BLOCK_MAX_COMPLEN)
                return -1;

            mNeededIn = std::max<size_t>(mNeededIn, out.curQuantumCompLen);
        }
    }

    // Always try and request slightly more than necessary, to avoid wasting DecomeSome calls
    mNeededIn = std::min<size_t>(mNeededIn + 16, OODLELZ_BLOCK_MAX_COMPLEN);

    return 0;
}
