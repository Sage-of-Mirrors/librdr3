#include "vertexdata.hpp"
#include "bstream.h"


/* Constants */

const uint32_t VTX_ATR_MAX = 52;


/* UVertexFormat */

void UVertexFormat::Deserialize(bStream::CStream* stream) {
    for (int i = 0; i < VTX_ATR_MAX; i++) {
        mOffsets[i] = stream->readUInt32();
    }
    for (int i = 0; i < VTX_ATR_MAX; i++) {
        mSizes[i] = stream->readUInt8();
    }
    for (int i = 0; i < VTX_ATR_MAX; i++) {
        mTypes[i] = (EVertexFormat)stream->readUInt8();
    }

    mData = stream->readUInt32();
    m013C = stream->readUInt32();
}

void UVertexFormat::Serialize(bStream::CStream* stream) {
    for (int i = 0; i < VTX_ATR_MAX; i++) {
        stream->writeUInt32(mOffsets[i]);
    }
    for (int i = 0; i < VTX_ATR_MAX; i++) {
        stream->writeUInt8(mSizes[i]);
    }
    for (int i = 0; i < VTX_ATR_MAX; i++) {
        stream->writeUInt8((uint8_t)mTypes[i]);
    }

    stream->writeUInt32(mData);
    stream->writeUInt32(m013C);
}


/* UVertexBuffer */

UVertexBuffer::UVertexBuffer() : mVTable(0), mVertexCount(0), mVertexStride(0), m000E(0), mBindFlags(0), m0014(0),
    mVertexData(nullptr), m0020(0), m0028(0), mShaderResourceView(0)
{

}

UVertexBuffer::~UVertexBuffer() {
    delete[] mVertexData;
}

void UVertexBuffer::Deserialize(bStream::CStream* stream) {
    mVTable = stream->readUInt64();

    mVertexCount = stream->readUInt32();
    mVertexStride = stream->readUInt16();
    m000E = stream->readUInt16();

    mBindFlags = stream->readUInt32();
    m0014 = stream->readUInt32();

    uint64_t vertexDataPtr = stream->readUInt64();
    m0020 = stream->readUInt64();
    m0028 = stream->readUInt64();

    uint64_t shaderResViewPtr = stream->readUInt64();
    uint64_t vertexFormatPtr = stream->readUInt64();

    // Read vertex format
    stream->seek(vertexFormatPtr & 0x0FFFFFFF);
    mVertexFormat.Deserialize(stream);

    // Read vertex data
    uint64_t bufferSize = mVertexCount * mVertexStride;
    mVertexData = new uint8_t[bufferSize];

    stream->seek(vertexDataPtr & 0x0FFFFFFF);
    stream->readBytesTo(mVertexData, bufferSize);
}

void UVertexBuffer::Serialize(bStream::CStream* stream) {

}

float UVertexBuffer::ReadVertexComponent(bStream::CStream& stream, EVertexFormat format) {
    float result = 0.0f;

    switch (format) {
        case EVertexFormat::VFT_RGBA32_FLOAT:
        case EVertexFormat::VFT_RGB32_FLOAT:
        case EVertexFormat::VFT_RG32:
            result = stream.readFloat();
            break;
        case EVertexFormat::VFT_RGBA16_FLOAT:
        case EVertexFormat::VFT_RG16_FLOAT:
            result = (float)stream.readUInt16();
            break;
        case EVertexFormat::VFT_RGB10A2:
            break;
        case EVertexFormat::VFT_RGBA8_UNORM:
            result = (float)stream.readUInt8() / 255.0f;
            break;
        case EVertexFormat::VFT_RGBA8_UINT:
            result = (float)stream.readUInt8();
            break;
        case EVertexFormat::VFT_NONE:
        default:
            break;
    }

    return result;
}

std::vector<UVertex> UVertexBuffer::GetVertices() {
    std::vector<UVertex> vertices;
    bStream::CMemoryStream bufferStream(mVertexData, mVertexCount * mVertexStride, bStream::Little, bStream::In);

    for (int vtx = 0; vtx < mVertexCount; vtx++) {
        UVertex newVertex;

        for (int atr = 0; atr < VTX_ATR_MAX; atr++) {
            if (mVertexFormat.mSizes[atr] == 0) {
                continue;
            }

            bufferStream.seek(mVertexFormat.mOffsets[atr] + vtx * mVertexFormat.mSizes[atr]);

            switch ((EVertexAttribute)atr) {
                case EVertexAttribute::VAT_POSITION:
                case EVertexAttribute::VAT_POSITION1:
                case EVertexAttribute::VAT_POSITION2:
                case EVertexAttribute::VAT_POSITION3:
                {
                    newVertex.Position[atr].x = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Position[atr].z = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Position[atr].y = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);

                    break;
                }
                case EVertexAttribute::VAT_NORMAL:
                case EVertexAttribute::VAT_NORMAL1:
                case EVertexAttribute::VAT_NORMAL2:
                case EVertexAttribute::VAT_NORMAL3:
                {
                    uint32_t nrmIndex = atr - (uint32_t)EVertexAttribute::VAT_NORMAL;

                    newVertex.Normal[nrmIndex].x = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Normal[nrmIndex].z = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Normal[nrmIndex].y = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);

                    break;
                }
                case EVertexAttribute::VAT_TANGENT:
                case EVertexAttribute::VAT_TANGENT1:
                case EVertexAttribute::VAT_TANGENT2:
                case EVertexAttribute::VAT_TANGENT3:
                {
                    uint32_t tgtIndex = atr - (uint32_t)EVertexAttribute::VAT_TANGENT;

                    newVertex.Tangent[tgtIndex].x = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Tangent[tgtIndex].z = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Tangent[tgtIndex].y = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);

                    break;
                }
                case EVertexAttribute::VAT_BINORMAL:
                case EVertexAttribute::VAT_BINORMAL1:
                case EVertexAttribute::VAT_BINORMAL2:
                case EVertexAttribute::VAT_BINORMAL3:
                {
                    uint32_t binIndex = atr - (uint32_t)EVertexAttribute::VAT_BINORMAL;

                    newVertex.Binormal[binIndex].x = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Binormal[binIndex].z = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Binormal[binIndex].y = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);

                    break;
                }
                case EVertexAttribute::VAT_BLENDWEIGHT:
                case EVertexAttribute::VAT_BLENDWEIGHT1:
                case EVertexAttribute::VAT_BLENDWEIGHT2:
                case EVertexAttribute::VAT_BLENDWEIGHT3:
                {
                    uint32_t bwtIndex = atr - (uint32_t)EVertexAttribute::VAT_BLENDWEIGHT;

                    newVertex.BlendWeight[bwtIndex].x = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.BlendWeight[bwtIndex].y = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.BlendWeight[bwtIndex].z = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.BlendWeight[bwtIndex].w = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);

                    break;
                }
                case EVertexAttribute::VAT_BLENDINDEX:
                case EVertexAttribute::VAT_BLENDINDEX1:
                case EVertexAttribute::VAT_BLENDINDEX2:
                case EVertexAttribute::VAT_BLENDINDEX3:
                {
                    uint32_t bidIndex = atr - (uint32_t)EVertexAttribute::VAT_BLENDINDEX;

                    newVertex.BlendIndex[bidIndex].x = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.BlendIndex[bidIndex].y = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.BlendIndex[bidIndex].z = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.BlendIndex[bidIndex].w = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);

                    break;
                }
                case EVertexAttribute::VAT_COLOR:
                case EVertexAttribute::VAT_COLOR1:
                case EVertexAttribute::VAT_COLOR2:
                case EVertexAttribute::VAT_COLOR3:
                {
                    uint32_t colIndex = atr - (uint32_t)EVertexAttribute::VAT_COLOR;

                    newVertex.Color[colIndex].x = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Color[colIndex].y = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Color[colIndex].z = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.Color[colIndex].w = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);

                    break;
                }
                case EVertexAttribute::VAT_TEXCOORD:
                case EVertexAttribute::VAT_TEXCOORD1:
                case EVertexAttribute::VAT_TEXCOORD2:
                case EVertexAttribute::VAT_TEXCOORD3:
                case EVertexAttribute::VAT_TEXCOORD4:
                case EVertexAttribute::VAT_TEXCOORD5:
                case EVertexAttribute::VAT_TEXCOORD6:
                case EVertexAttribute::VAT_TEXCOORD7:
                case EVertexAttribute::VAT_TEXCOORD8:
                case EVertexAttribute::VAT_TEXCOORD9:
                case EVertexAttribute::VAT_TEXCOORD10:
                case EVertexAttribute::VAT_TEXCOORD11:
                case EVertexAttribute::VAT_TEXCOORD12:
                case EVertexAttribute::VAT_TEXCOORD13:
                case EVertexAttribute::VAT_TEXCOORD14:
                case EVertexAttribute::VAT_TEXCOORD15:
                case EVertexAttribute::VAT_TEXCOORD16:
                case EVertexAttribute::VAT_TEXCOORD17:
                case EVertexAttribute::VAT_TEXCOORD18:
                case EVertexAttribute::VAT_TEXCOORD19:
                case EVertexAttribute::VAT_TEXCOORD20:
                case EVertexAttribute::VAT_TEXCOORD21:
                case EVertexAttribute::VAT_TEXCOORD22:
                case EVertexAttribute::VAT_TEXCOORD23:
                {
                    uint32_t txcIndex = atr - (uint32_t)EVertexAttribute::VAT_TEXCOORD;

                    newVertex.TexCoord[txcIndex].x = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);
                    newVertex.TexCoord[txcIndex].y = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);

                    break;
                }
            }
        }

        vertices.push_back(newVertex);
    }

    return vertices;
}


/* UIndexBuffer */

UIndexBuffer::UIndexBuffer() : mVTable(0), mIndexCount(0), mIndexStride(0), m000E(0), mFlags(0), m0014(0),
    mIndexData(nullptr), m0020(0), m0028(0), mShaderResourceView(0), m0038(0), m0040(0), m0048(0)
{

}

UIndexBuffer::~UIndexBuffer() {
    if (mIndexData != nullptr) {
        delete[] mIndexData;
        mIndexData = nullptr;
    }
}

void UIndexBuffer::Deserialize(bStream::CStream* stream) {
    mVTable = stream->readUInt64();

    mIndexCount = stream->readUInt32();
    mIndexStride = stream->readUInt16();
    m000E = stream->readUInt16();

    mFlags = stream->readUInt32();
    m0014 = stream->readUInt32();

    uint64_t indexDataPtr = stream->readUInt64();
    m0020 = stream->readUInt64();
    m0028 = stream->readUInt64();

    uint64_t shaderResViewPtr = stream->readUInt64();
    uint64_t somePtr = stream->readUInt64();

    // Read index data
    uint64_t bufferSize = mIndexCount * mIndexStride;
    mIndexData = new uint8_t[bufferSize];

    stream->seek(indexDataPtr & 0x0FFFFFFF);
    stream->readBytesTo(mIndexData, bufferSize);
}

void UIndexBuffer::Serialize(bStream::CStream* stream) {

}

std::vector<uint32_t> UIndexBuffer::GetIndices() {
    std::vector<uint32_t> indices;
    bStream::CMemoryStream bufferStream(mIndexData, mIndexCount * mIndexStride, bStream::Little, bStream::In);

    for (int i = 0; i < mIndexCount; i++) {
        switch (mIndexStride) {
            case 1: // Byte
                indices.push_back(bufferStream.readUInt8());
                break;
            case 2: // Short
                indices.push_back(bufferStream.readUInt16());
                break;
            case 4: // Int32
                indices.push_back(bufferStream.readUInt32());
                break;
        }
    }

    return indices;
}
