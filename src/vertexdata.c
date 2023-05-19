#include "vertexdata.h"
#include "stream.h"

#include <stdio.h>

const uint32_t VTX_ATR_MAX = 4;
const uint32_t VTX_ATR_TXCRD_MAX = 24;

void DeserializeVertexBuffer(UStream* stream, UVertexBuffer* dst) {
    dst->mVTable = UStreamReadU64(stream);

    dst->mVertexCount = UStreamReadU32(stream);
    dst->mVertexStride = UStreamReadU16(stream);
    dst->m000E = UStreamReadU16(stream);

    dst->mBindFlags = UStreamReadU32(stream);
    dst->m0014 = UStreamReadU32(stream);

    uint64_t vertexDataPtr = UStreamReadU64(stream);
    dst->m0020 = UStreamReadU64(stream);
    dst->m0028 = UStreamReadU64(stream);

    uint64_t shaderResViewPtr = UStreamReadU64(stream);
    uint64_t vertexFormatPtr = UStreamReadU64(stream);

    // Read vertex format
    dst->mVertexFormat = (UVertexFormat*)malloc(sizeof(UVertexFormat));

    UStreamSeek(stream, vertexFormatPtr - 0x50000000, 0);
    DeserializeVertexFormat(stream, dst->mVertexFormat);

    // Read vertex data
    uint64_t bufferSize = dst->mVertexCount * dst->mVertexStride;
    dst->mVertexData = (uint8_t*)malloc(bufferSize);
    memcpy(dst->mVertexData, stream->buffer + (vertexDataPtr - 0x50000000), bufferSize);

    // DEBUG
    //UVertex* test = GetVertices(dst);

    //FILE* testObj = fopen("D:\\RedDead\\libtest.obj", "a");
    
    //for (int i = 0; i < dst->mVertexCount; i++) {
    //    fprintf(testObj, "v %f %f %f\n", test[i].mPosition[0].x, test[i].mPosition[0].y, test[i].mPosition[0].z);
    //}

    //fclose(testObj);
}

void SerializeVertexBuffer(UStream* stream, UVertexBuffer* src) {

}

void DeserializeIndexBuffer(UStream* stream, UIndexBuffer* dst) {

}

void SerializeIndexBuffer(UStream* stream, UIndexBuffer* src) {

}

void DeserializeVertexFormat(UStream* stream, UVertexFormat* dst) {
    for (int i = 0; i < 52; i++) {
        dst->mOffsets[i] = UStreamReadU32(stream);
    }
    for (int i = 0; i < 52; i++) {
        dst->mSizes[i] = UStreamReadU8(stream);
    }
    for (int i = 0; i < 52; i++) {
        dst->mTypes[i] = UStreamReadU8(stream);
    }

    dst->mData = UStreamReadU32(stream);
    dst->m013C = UStreamReadU32(stream);
}

void SerializeVertexFormat(UStream* stream, UVertexFormat* src) {

}

float ReadVertexComponent(UStream* stream, EVertexFormat fmt) {
    if (stream == NULL) {
        return 0.0f;
    }

    float result = 0.0f;

    switch (fmt) {
    case VFT_RGBA32_FLOAT:
    case VFT_RGB32_FLOAT:
    case VFT_RG32:
        result = UStreamReadFloat(stream);
        break;
    case VFT_RGBA16_FLOAT:
    case VFT_RG16_FLOAT:
        result = (float)UStreamReadU16(stream);
        break;
    case VFT_RGB10A2:
        break;
    case VFT_RGBA8_UNORM:
        result = (float)UStreamReadU8(stream) / 255.0f;
        break;
    case VFT_RGBA8_UINT:
        result = (float)UStreamReadU8(stream);
        break;
    case VFT_NONE:
    default:
        break;
    }

    return result;
}

UVertex* GetVertices(const UVertexBuffer* buf) {
    if (buf == NULL) {
        return NULL;
    }

    UVertex* vertices = (UVertex*)malloc(sizeof(UVertex) * buf->mVertexCount);
    UStream* bufferStream = (UStream*)malloc(sizeof(UStream));
    UVertexFormat* fmt = buf->mVertexFormat;

    UInitStream(bufferStream, buf->mVertexData, buf->mVertexCount * buf->mVertexStride, ENDIAN_LITTLE);

    for (int vtx = 0; vtx < buf->mVertexCount; vtx++) {
        // Position
        for (int pos = 0; pos < VTX_ATR_MAX; pos++) {
            if (fmt->mSizes[pos] == 0) {
                continue;
            }

            UStreamSeek(bufferStream, fmt->mOffsets[pos] + vtx * fmt->mSizes[pos], 0);

            vertices[vtx].mPosition[pos].x = ReadVertexComponent(bufferStream, fmt->mTypes[pos]);
            vertices[vtx].mPosition[pos].z = ReadVertexComponent(bufferStream, fmt->mTypes[pos]);
            vertices[vtx].mPosition[pos].y = ReadVertexComponent(bufferStream, fmt->mTypes[pos]);
        }
        // Normal
        for (int nrm = 0; nrm < VTX_ATR_MAX; nrm++) {
            int nrmIdx = nrm + 4;

            if (fmt->mSizes[nrmIdx] == 0) {
                continue;
            }

            UStreamSeek(bufferStream, fmt->mOffsets[nrmIdx] + vtx * fmt->mSizes[nrmIdx], 0);

            vertices[vtx].mNormal[nrm].x = ReadVertexComponent(bufferStream, fmt->mTypes[nrmIdx]);
            vertices[vtx].mNormal[nrm].z = ReadVertexComponent(bufferStream, fmt->mTypes[nrmIdx]);
            vertices[vtx].mNormal[nrm].y = ReadVertexComponent(bufferStream, fmt->mTypes[nrmIdx]);
        }
        // Tangent
        for (int tgt = 0; tgt < VTX_ATR_MAX; tgt++) {
            int tgtIdx = tgt + 8;

            if (fmt->mSizes[tgtIdx] == 0) {
                continue;
            }

            UStreamSeek(bufferStream, fmt->mOffsets[tgtIdx] + vtx * fmt->mSizes[tgtIdx], 0);

            vertices[vtx].mTangent[tgt].x = ReadVertexComponent(bufferStream, fmt->mTypes[tgtIdx]);
            vertices[vtx].mTangent[tgt].z = ReadVertexComponent(bufferStream, fmt->mTypes[tgtIdx]);
            vertices[vtx].mTangent[tgt].y = ReadVertexComponent(bufferStream, fmt->mTypes[tgtIdx]);
        }
        // Binormal
        for (int bnm = 0; bnm < VTX_ATR_MAX; bnm++) {
            int bnmIdx = bnm + 12;

            if (fmt->mSizes[bnmIdx] == 0) {
                continue;
            }

            UStreamSeek(bufferStream, fmt->mOffsets[bnmIdx] + vtx * fmt->mSizes[bnmIdx], 0);

            vertices[vtx].mBinormal[bnm].x = ReadVertexComponent(bufferStream, fmt->mTypes[bnmIdx]);
            vertices[vtx].mBinormal[bnm].z = ReadVertexComponent(bufferStream, fmt->mTypes[bnmIdx]);
            vertices[vtx].mBinormal[bnm].y = ReadVertexComponent(bufferStream, fmt->mTypes[bnmIdx]);
        }
        // Blend weight
        for (int blw = 0; blw < VTX_ATR_MAX; blw++) {
            int blwIdx = blw + 16;

            if (fmt->mSizes[blwIdx] == 0) {
                continue;
            }

            UStreamSeek(bufferStream, fmt->mOffsets[blwIdx] + vtx * fmt->mSizes[blwIdx], 0);

            vertices[vtx].mBlendWeight[blw].x = ReadVertexComponent(bufferStream, fmt->mTypes[blwIdx]);
            vertices[vtx].mBlendWeight[blw].y = ReadVertexComponent(bufferStream, fmt->mTypes[blwIdx]);
            vertices[vtx].mBlendWeight[blw].z = ReadVertexComponent(bufferStream, fmt->mTypes[blwIdx]);
            vertices[vtx].mBlendWeight[blw].w = ReadVertexComponent(bufferStream, fmt->mTypes[blwIdx]);
        }
        // Blend index
        for (int bli = 0; bli < VTX_ATR_MAX; bli++) {
            int bliIdx = bli + 20;

            if (fmt->mSizes[bliIdx] == 0) {
                continue;
            }

            UStreamSeek(bufferStream, fmt->mOffsets[bliIdx] + vtx * fmt->mSizes[bliIdx], 0);

            vertices[vtx].mBlendIndex[bli].x = ReadVertexComponent(bufferStream, fmt->mTypes[bliIdx]);
            vertices[vtx].mBlendIndex[bli].y = ReadVertexComponent(bufferStream, fmt->mTypes[bliIdx]);
            vertices[vtx].mBlendIndex[bli].z = ReadVertexComponent(bufferStream, fmt->mTypes[bliIdx]);
            vertices[vtx].mBlendIndex[bli].w = ReadVertexComponent(bufferStream, fmt->mTypes[bliIdx]);
        }
        // Color
        for (int col = 0; col < VTX_ATR_MAX; col++) {
            int colIdx = col + 24;

            if (fmt->mSizes[colIdx] == 0) {
                continue;
            }

            UStreamSeek(bufferStream, fmt->mOffsets[colIdx] + vtx * fmt->mSizes[colIdx], 0);

            vertices[vtx].mColor[col].x = ReadVertexComponent(bufferStream, fmt->mTypes[colIdx]);
            vertices[vtx].mColor[col].y = ReadVertexComponent(bufferStream, fmt->mTypes[colIdx]);
            vertices[vtx].mColor[col].z = ReadVertexComponent(bufferStream, fmt->mTypes[colIdx]);
            vertices[vtx].mColor[col].w = ReadVertexComponent(bufferStream, fmt->mTypes[colIdx]);
        }
        // Tex coord
        for (int txc = 0; txc < VTX_ATR_TXCRD_MAX; txc++) {
            int txcIdx = txc + 28;

            if (fmt->mSizes[txcIdx] == 0) {
                continue;
            }

            UStreamSeek(bufferStream, fmt->mOffsets[txcIdx] + vtx * fmt->mSizes[txcIdx], 0);

            vertices[vtx].mColor[txc].x = ReadVertexComponent(bufferStream, fmt->mTypes[txcIdx]);
            vertices[vtx].mColor[txc].y = ReadVertexComponent(bufferStream, fmt->mTypes[txcIdx]);
        }
    }

    free(bufferStream);
    return vertices;
}
