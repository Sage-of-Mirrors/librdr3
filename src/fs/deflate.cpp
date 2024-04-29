// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/asset/transform/deflate.cpp

#include "fs/deflate.hpp"

namespace {
    static void* Z_alloc([[maybe_unused]] void* opaque, unsigned int items, unsigned int size)
    {
        return operator new(size * items);
    }

    static void Z_free([[maybe_unused]] void* opaque, void* address)
    {
        operator delete(address);
    }
}

rdr3::fs::CDeflateDecompressor::CDeflateDecompressor(int32_t windowBits) {
    mInflater.zalloc = Z_alloc;
    mInflater.zfree = Z_free;
    //zng_inflateInit2(&mInflater, windowBits);
}

rdr3::fs::CDeflateDecompressor::~CDeflateDecompressor()
{
    //zng_inflateEnd(&mInflater);
}

bool rdr3::fs::CDeflateDecompressor::Reset()
{
    return true; //zng_inflateReset(&mInflater) == Z_OK;
}

bool rdr3::fs::CDeflateDecompressor::Update(uint8_t* outData, uint8_t* inData, size_t outSize, size_t inSize)
{
    int error = Z_OK;

    // Try and inflate as much data as possible
    while ((error == Z_OK))
    {
        uInt actual_in  = (inSize <= UINT_MAX) ? uInt(inSize) : UINT_MAX;
        uInt actual_out = (outSize <= UINT_MAX) ? uInt(outSize) : UINT_MAX;

        mInflater.next_in  = inData;
        mInflater.next_out = outData;

        mInflater.avail_in  = actual_in;
        mInflater.avail_out = actual_out;

        // Always try and flush as much data as possible
        //error = zng_inflate(&mInflater, Z_SYNC_FLUSH);

        inSize -= actual_in - mInflater.avail_in;
        outSize -= actual_out - mInflater.avail_out;
    }

    // No progress was possible (avail_in or avail_out was zero)
    if (error == Z_BUF_ERROR)
    {
        // We have sufficient output capacity, but there is no more input
        if (outSize && inSize)
            return false;

        // The stream is valid, but just needs more input/output
        return true;
    }

    // The end of the compressed data has been reached and all uncompressed output has been produced
    if (error == Z_STREAM_END)
    {
        return true;
    }

    return error == Z_OK;
}
