/* On loan from https://github.com/Astral-C/libgctools/blob/master/src/stream.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stream.h"

// Swap Functions

uint32_t UStreamSwap16(uint32_t n) {
    return (((n >> 8) & 0x00FF) |
            ((n << 8) & 0xFF00));
}

uint32_t UStreamSwap32(uint32_t n) {
    return (((n >> 24) & 0x000000FF) |
            ((n >> 8)  & 0x0000FF00) |
            ((n << 8)  & 0x00FF0000) |
            ((n << 24) & 0xFF000000));
}

uint64_t UStreamSwap64(uint64_t n) {
    return (((n >> 56) & 0x00000000000000FF) |
            ((n >> 40) & 0x000000000000FF00) |
            ((n >> 24) & 0x0000000000FF0000) |
            ((n >> 8)  & 0x00000000FF000000) |
            ((n << 8)  & 0x000000FF00000000) |
            ((n << 24) & 0x0000FF0000000000) |
            ((n << 40) & 0x00FF000000000000) |
            ((n << 56) & 0xFF00000000000000));
}

// Management Functions

UStreamError UInitStream(UStream* stream, const void* buffer, size_t size, UStreamEndian endian) {

    stream->buffer = buffer;
    stream->position = 0;
    stream->size = size;
    stream->endian = endian;
    stream->systemOrder = ENDIAN_LITTLE;

    return STREAM_ERROR_SUCCESS;
}

UStreamError UStreamSeek(UStream* stream, uint32_t pos, uint32_t from) {
    if(pos > stream->size || from > stream->size || from < 0 || pos < 0){
        return STREAM_ERROR_OOB;
    }

    stream->position = from + pos;
    return STREAM_ERROR_SUCCESS;
}

// Reader Functions

int8_t UStreamRead8(UStream* stream) {
    int8_t r = *((int8_t*)UOffsetPointer(stream->buffer, stream->position));
    stream->position++;
    return r;
}

uint8_t UStreamReadU8(UStream* stream) {
    uint8_t r = *((uint8_t*)UOffsetPointer(stream->buffer, stream->position));
    stream->position++;
    return r;
}

int16_t UStreamRead16(UStream* stream) {
    int16_t r;
    memcpy(&r, UOffsetPointer(stream->buffer, stream->position), sizeof(int16_t));
    stream->position += sizeof(int16_t);
    return (stream->endian != stream->systemOrder ? UStreamSwap16(r) : r);
}

uint16_t UStreamReadU16(UStream* stream) {
    uint16_t r;
    memcpy(&r, UOffsetPointer(stream->buffer, stream->position), sizeof(uint16_t));
    stream->position += sizeof(uint16_t);
    return (stream->endian != stream->systemOrder ? UStreamSwap16(r) : r);
}

int32_t UStreamRead32(UStream* stream) {
    int32_t r;
    memcpy(&r, UOffsetPointer(stream->buffer, stream->position), sizeof(int32_t));
    stream->position += sizeof(int32_t);
    return (stream->endian != stream->systemOrder ? UStreamSwap32(r) : r);
}

uint32_t UStreamReadU32(UStream* stream) {
    uint32_t r;
    memcpy(&r, UOffsetPointer(stream->buffer, stream->position), sizeof(uint32_t));
    stream->position += sizeof(uint32_t);
    return (stream->endian != stream->systemOrder ? UStreamSwap32(r) : r);
}

int64_t UStreamRead64(UStream* stream) {
    int64_t r;
    memcpy(&r, UOffsetPointer(stream->buffer, stream->position), sizeof(int64_t));
    stream->position += sizeof(int64_t);
    return (stream->endian != stream->systemOrder ? UStreamSwap64(r) : r);
}

uint64_t UStreamReadU64(UStream* stream) {
    uint64_t r;
    memcpy(&r, UOffsetPointer(stream->buffer, stream->position), sizeof(uint64_t));
    stream->position += sizeof(uint64_t);
    return (stream->endian != stream->systemOrder ? UStreamSwap64(r) : r);
}

float UStreamReadFloat(UStream* stream) {
    float r;
    memcpy(&r, UOffsetPointer(stream->buffer, stream->position), sizeof(float));
    stream->position += sizeof(float);
    return (stream->endian != stream->systemOrder ? UStreamSwap32(r) : r);
}

double UStreamReadDouble(UStream* stream) {
    double r;
    memcpy(&r, UOffsetPointer(stream->buffer, stream->position), sizeof(double));
    stream->position += sizeof(double);
    return (stream->endian != stream->systemOrder ? UStreamSwap64(r) : r);
}

char* UStreamReadStr(UStream* stream, size_t size) {
    char* str = (char*)malloc(size);
    if (str == NULL) {
        return NULL;
    }

    strncpy(str, (char*)UOffsetPointer(stream->buffer, stream->position), size);
    stream->position += size;
    return str;
}

// Writer Functions

void UStreamWrite8(UStream* stream, int8_t r) {
    *((int8_t*)UOffsetPointer(stream->buffer, stream->position)) = r;
    stream->position++;
}

void UStreamWriteU8(UStream* stream, uint8_t r) {
    *((uint8_t*)UOffsetPointer(stream->buffer, stream->position)) = r;
    stream->position++;
}

void UStreamWrite16(UStream* stream, int16_t r) {
    *((int16_t*)UOffsetPointer(stream->buffer, stream->position)) = (stream->endian == ENDIAN_LITTLE ? r : UStreamSwap16(r));
    stream->position += sizeof(int16_t);
}

void UStreamWriteU16(UStream* stream, uint16_t r) {
    *((uint16_t*)UOffsetPointer(stream->buffer, stream->position)) = (stream->endian == ENDIAN_LITTLE ? r : UStreamSwap16(r));
    stream->position += sizeof(uint16_t);
}

void UStreamWrite32(UStream* stream, int32_t r) {
    *((int32_t*)UOffsetPointer(stream->buffer, stream->position)) = (stream->endian == ENDIAN_LITTLE ? r : UStreamSwap32(r));
    stream->position += sizeof(int32_t);
}

void UStreamWriteU32(UStream* stream, uint32_t r) {
    *((uint32_t*)UOffsetPointer(stream->buffer, stream->position)) = (stream->endian == ENDIAN_LITTLE ? r : UStreamSwap32(r));
    stream->position += sizeof(uint32_t);
}

void UStreamWrite64(UStream* stream, int64_t r) {
    *((int64_t*)UOffsetPointer(stream->buffer, stream->position)) = (stream->endian == ENDIAN_LITTLE ? r : UStreamSwap64(r));
    stream->position += sizeof(int64_t);
}

void UStreamWriteU64(UStream* stream, uint64_t r) {
    *((uint64_t*)UOffsetPointer(stream->buffer, stream->position)) = (stream->endian == ENDIAN_LITTLE ? r : UStreamSwap64(r));
    stream->position += sizeof(uint64_t);
}

void UStreamWriteFloat(UStream* stream, float r) {
    *((float*)UOffsetPointer(stream->buffer, stream->position)) = (stream->endian == ENDIAN_LITTLE ? r : UStreamSwap32(r));
    stream->position += sizeof(float);
}

void UStreamWriteDouble(UStream* stream, double r) {
    *((double*)UOffsetPointer(stream->buffer, stream->position)) = (stream->endian == ENDIAN_LITTLE ? r : UStreamSwap64(r));
    stream->position += sizeof(double);
}

void UStreamWriteStr(UStream* stream, const char* str, size_t size) {
    strncpy((char*)UOffsetPointer(stream->buffer, stream->position), str, size);
    stream->position += size;
}
