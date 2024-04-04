#include "drawable/vertexdata.hpp"
#include "util/bstream.h"


/* Constants */

const uint32_t VTX_ATR_MAX = 52;


/* CVertexFormat */

void CVertexFormatData::Deserialize(bStream::CStream* stream) {
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

void CVertexFormatData::Serialize(bStream::CMemoryStream* stream) {
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

CVertexBuffer::CVertexBuffer() : mVTable(0), mVertexCount(0), mVertexStride(0), m000E(0), mBindFlags(0), m0014(0),
    mVertexData(nullptr), m0020(0), m0028(0), mShaderResourceView(0)
{

}

CVertexBuffer::~CVertexBuffer() {
    delete[] mVertexData;
}

void CVertexBuffer::Deserialize(bStream::CStream* stream) {
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

void CVertexBuffer::Serialize(bStream::CMemoryStream* stream) {
    // Write vertex buffer data
    uint64_t vertexBufferOffset = stream->getSize();
    stream->seek(vertexBufferOffset);
    stream->writeBytesTo(mVertexData, mVertexCount * mVertexStride);

    // Write shader resource view
    // TODO: implement
    uint64_t shaderResourceViewOffset = stream->getSize();

    // Write vertex format data
    uint64_t vertexFormatOffset = stream->getSize();
    mVertexFormat.Serialize(stream);

    // Write vertex buffer metadata
    stream->writeUInt64(mVTable);
    stream->writeUInt32(mVertexCount);
    stream->writeUInt16(mVertexStride);
    stream->writeUInt16(m000E);

    stream->writeUInt32(mBindFlags);
    stream->writeUInt32(m0014);

    stream->writeUInt64(vertexBufferOffset | 0x50000000);
    stream->writeUInt64(m0020);
    stream->writeUInt64(m0028);

    stream->writeUInt64(shaderResourceViewOffset | 0x50000000);
    stream->writeUInt64(vertexFormatOffset | 0x50000000);
}

std::array<float, 4> CVertexBuffer::ReadVertexComponent(bStream::CStream& stream, EVertexFormat format) {
    switch (format) {
        case EVertexFormat::RGBA32_FLOAT:
        {
            return {
                stream.readFloat(),
                stream.readFloat(),
                stream.readFloat(),
                stream.readFloat()
            };
        }
        case EVertexFormat::RGB32_FLOAT:
        {
            return {
                stream.readFloat(),
                stream.readFloat(),
                stream.readFloat(),
                0.0f
            };
        }
        case EVertexFormat::RG32:
        {
            return {
                stream.readFloat(),
                stream.readFloat(),
                0.0f,
                0.0f
            };
        }
        case EVertexFormat::RGBA16_FLOAT:
        {
            return {
                static_cast<float>(stream.readUInt16()),
                static_cast<float>(stream.readUInt16()),
                static_cast<float>(stream.readUInt16()),
                static_cast<float>(stream.readUInt16())
            };
        }
        case EVertexFormat::RG16_FLOAT:
        {
            return {
                static_cast<float>(stream.readUInt16()),
                static_cast<float>(stream.readUInt16()),
                0.0f,
                0.0f
            };
        }
        case EVertexFormat::RGB10A2:
        {
            uint32_t data = stream.readUInt32();
            return {
                ((static_cast<float>(data & 0x3FF) / 1023.0f) * 2) - 1,
                ((static_cast<float>(((data >> 10) & 0x3FF)) / 1023.0f) * 2) - 1,
                ((static_cast<float>(((data >> 20) & 0x3FF)) / 1023.0f) * 2) - 1,
                0.0f
            };
        }
        case EVertexFormat::RGBA8_UNORM:
        {
            return {
                static_cast<float>(stream.readUInt8()) / 255.0f,
                static_cast<float>(stream.readUInt8()) / 255.0f,
                static_cast<float>(stream.readUInt8()) / 255.0f,
                static_cast<float>(stream.readUInt8()) / 255.0f
            };
        }
        case EVertexFormat::RGBA8_UINT:
        {
            return {
                static_cast<float>(stream.readUInt8()),
                static_cast<float>(stream.readUInt8()),
                static_cast<float>(stream.readUInt8()),
                static_cast<float>(stream.readUInt8())
            };
        }
        case EVertexFormat::NONE:
        default:
        {
            return {
                0.0f,
                0.0f,
                0.0f,
                0.0f
            };
        }
    }
}

void CVertexBuffer::WriteVertexComponent(bStream::CStream* stream, EVertexFormat format, std::array<float, 4> data) {
    switch (format) {
        case EVertexFormat::RGBA32_FLOAT:
        {
            stream->writeFloat(data[0]);
            stream->writeFloat(data[1]);
            stream->writeFloat(data[2]);
            stream->writeFloat(data[3]);

            return;
        }
        case EVertexFormat::RGB32_FLOAT:
        {
            stream->writeFloat(data[0]);
            stream->writeFloat(data[1]);
            stream->writeFloat(data[2]);

            return;
        }
        case EVertexFormat::RG32:
        {
            stream->writeFloat(data[0]);
            stream->writeFloat(data[1]);

            return;
        }
        case EVertexFormat::RGBA16_FLOAT:
        {
            stream->writeUInt16(static_cast<uint16_t>(data[0]));
            stream->writeUInt16(static_cast<uint16_t>(data[1]));
            stream->writeUInt16(static_cast<uint16_t>(data[2]));
            stream->writeUInt16(static_cast<uint16_t>(data[3]));

            return;
        }
        case EVertexFormat::RG16_FLOAT:
        {
            stream->writeUInt16(static_cast<uint16_t>(data[0]));
            stream->writeUInt16(static_cast<uint16_t>(data[1]));

            return;
        }
        case EVertexFormat::RGB10A2:
        {
            uint32_t field = 0;
            field |= static_cast<uint32_t>(((data[0]  + 1) / 2) * 1023.0f) & 0x3FF;
            field |= (static_cast<uint32_t>(((data[1] + 1) / 2) * 1023.0f) & 0x3FF) << 10;
            field |= (static_cast<uint32_t>(((data[2] + 1) / 2) * 1023.0f) & 0x3FF) << 20;

            stream->writeUInt32(field);
            return;
        }
        case EVertexFormat::RGBA8_UNORM:
        {
            stream->writeUInt8(static_cast<uint8_t>(data[0] * 255.f));
            stream->writeUInt8(static_cast<uint8_t>(data[1] * 255.f));
            stream->writeUInt8(static_cast<uint8_t>(data[2] * 255.f));
            stream->writeUInt8(static_cast<uint8_t>(data[3] * 255.f));
            
            return;
        }
        case EVertexFormat::RGBA8_UINT:
        {
            stream->writeUInt8(static_cast<uint8_t>(data[0]));
            stream->writeUInt8(static_cast<uint8_t>(data[1]));
            stream->writeUInt8(static_cast<uint8_t>(data[2]));
            stream->writeUInt8(static_cast<uint8_t>(data[3]));

            return;
        }
        case EVertexFormat::NONE:
        default:
        {
            return;
        }
    }
}

shared_vector<CVertex> CVertexBuffer::GetVertices() {
    shared_vector<CVertex> vertices;
    bStream::CMemoryStream bufferStream(mVertexData, mVertexCount * mVertexStride, bStream::Little, bStream::In);

    for (int vtx = 0; vtx < mVertexCount; vtx++) {
        std::shared_ptr<CVertex> newVertex = std::make_shared<CVertex>();

        for (int atr = 0; atr < VTX_ATR_MAX; atr++) {
            if (mVertexFormat.mSizes[atr] == 0) {
                continue;
            }

            bufferStream.seek(mVertexFormat.mOffsets[atr] + vtx * mVertexFormat.mSizes[atr]);
            std::array<float, 4> vertData = ReadVertexComponent(bufferStream, mVertexFormat.mTypes[atr]);

            switch ((EVertexAttribute)atr) {
                case EVertexAttribute::POSITION:
                case EVertexAttribute::POSITION1:
                case EVertexAttribute::POSITION2:
                case EVertexAttribute::POSITION3:
                {
                    newVertex->Position[atr].x = vertData[0];
                    newVertex->Position[atr].y = vertData[1];
                    newVertex->Position[atr].z = vertData[2];

                    break;
                }
                case EVertexAttribute::NORMAL:
                case EVertexAttribute::NORMAL1:
                case EVertexAttribute::NORMAL2:
                case EVertexAttribute::NORMAL3:
                {
                    uint32_t nrmIndex = atr - (uint32_t)EVertexAttribute::NORMAL;

                    newVertex->Normal[nrmIndex].x = vertData[0];
                    newVertex->Normal[nrmIndex].y = vertData[1];
                    newVertex->Normal[nrmIndex].z = vertData[2];

                    break;
                }
                case EVertexAttribute::TANGENT:
                case EVertexAttribute::TANGENT1:
                case EVertexAttribute::TANGENT2:
                case EVertexAttribute::TANGENT3:
                {
                    uint32_t tgtIndex = atr - (uint32_t)EVertexAttribute::TANGENT;

                    newVertex->Tangent[tgtIndex].x = vertData[0];
                    newVertex->Tangent[tgtIndex].y = vertData[1];
                    newVertex->Tangent[tgtIndex].z = vertData[2];

                    break;
                }
                case EVertexAttribute::BINORMAL:
                case EVertexAttribute::BINORMAL1:
                case EVertexAttribute::BINORMAL2:
                case EVertexAttribute::BINORMAL3:
                {
                    uint32_t binIndex = atr - (uint32_t)EVertexAttribute::BINORMAL;

                    newVertex->Binormal[binIndex].x = vertData[0];
                    newVertex->Binormal[binIndex].y = vertData[1];
                    newVertex->Binormal[binIndex].z = vertData[2];

                    break;
                }
                case EVertexAttribute::BLENDWEIGHT:
                case EVertexAttribute::BLENDWEIGHT1:
                case EVertexAttribute::BLENDWEIGHT2:
                case EVertexAttribute::BLENDWEIGHT3:
                {
                    uint32_t bwtIndex = atr - (uint32_t)EVertexAttribute::BLENDWEIGHT;

                    newVertex->BlendWeight[bwtIndex].x = vertData[0];
                    newVertex->BlendWeight[bwtIndex].y = vertData[1];
                    newVertex->BlendWeight[bwtIndex].z = vertData[2];
                    newVertex->BlendWeight[bwtIndex].w = vertData[3];

                    break;
                }
                case EVertexAttribute::BLENDINDEX:
                case EVertexAttribute::BLENDINDEX1:
                case EVertexAttribute::BLENDINDEX2:
                case EVertexAttribute::BLENDINDEX3:
                {
                    uint32_t bidIndex = atr - (uint32_t)EVertexAttribute::BLENDINDEX;

                    newVertex->BlendIndex[bidIndex].x = vertData[0];
                    newVertex->BlendIndex[bidIndex].y = vertData[1];
                    newVertex->BlendIndex[bidIndex].z = vertData[2];
                    newVertex->BlendIndex[bidIndex].w = vertData[3];

                    break;
                }
                case EVertexAttribute::COLOR:
                case EVertexAttribute::COLOR1:
                case EVertexAttribute::COLOR2:
                case EVertexAttribute::COLOR3:
                {
                    uint32_t colIndex = atr - (uint32_t)EVertexAttribute::COLOR;

                    newVertex->Color[colIndex].x = vertData[0];
                    newVertex->Color[colIndex].y = vertData[1];
                    newVertex->Color[colIndex].z = vertData[2];
                    newVertex->Color[colIndex].w = vertData[3];

                    break;
                }
                case EVertexAttribute::TEXCOORD:
                case EVertexAttribute::TEXCOORD1:
                case EVertexAttribute::TEXCOORD2:
                case EVertexAttribute::TEXCOORD3:
                case EVertexAttribute::TEXCOORD4:
                case EVertexAttribute::TEXCOORD5:
                case EVertexAttribute::TEXCOORD6:
                case EVertexAttribute::TEXCOORD7:
                case EVertexAttribute::TEXCOORD8:
                case EVertexAttribute::TEXCOORD9:
                case EVertexAttribute::TEXCOORD10:
                case EVertexAttribute::TEXCOORD11:
                case EVertexAttribute::TEXCOORD12:
                case EVertexAttribute::TEXCOORD13:
                case EVertexAttribute::TEXCOORD14:
                case EVertexAttribute::TEXCOORD15:
                case EVertexAttribute::TEXCOORD16:
                case EVertexAttribute::TEXCOORD17:
                case EVertexAttribute::TEXCOORD18:
                case EVertexAttribute::TEXCOORD19:
                case EVertexAttribute::TEXCOORD20:
                case EVertexAttribute::TEXCOORD21:
                case EVertexAttribute::TEXCOORD22:
                case EVertexAttribute::TEXCOORD23:
                {
                    uint32_t txcIndex = atr - (uint32_t)EVertexAttribute::TEXCOORD;

                    newVertex->TexCoord[txcIndex].x = vertData[0];
                    newVertex->TexCoord[txcIndex].y = 1.0f - vertData[1];

                    break;
                }
            }
        }

        vertices.push_back(newVertex);
    }

    return vertices;
}

void CVertexBuffer::SetVertices(shared_vector<CVertex>& vertices, std::vector<EVertexFormat> formats) {
    mVertexCount = vertices.size();
    mVertexStride = CalculateVertexStride(formats);

    mVertexData = new uint8_t[mVertexCount * mVertexStride];
    bStream::CMemoryStream stream(mVertexData, mVertexCount * mVertexStride, bStream::Little, bStream::Out);

    for (std::shared_ptr<CVertex> v : vertices) {
        for (int atr = 0; atr < VTX_ATR_MAX; atr++) {
            if (formats[atr] == EVertexFormat::NONE) {
                continue;
            }

            std::array<float, 4> data;

            switch (static_cast<EVertexAttribute>(atr)) {
                case EVertexAttribute::POSITION:
                case EVertexAttribute::POSITION1:
                case EVertexAttribute::POSITION2:
                case EVertexAttribute::POSITION3:
                {
                    data = { v->Position[atr].x, v->Position[atr].y, v->Position[atr].z, 0.0f };
                    break;
                }
                case EVertexAttribute::NORMAL:
                case EVertexAttribute::NORMAL1:
                case EVertexAttribute::NORMAL2:
                case EVertexAttribute::NORMAL3:
                {
                    uint32_t nrmIndex = atr - static_cast<uint32_t>(EVertexAttribute::NORMAL);

                    data = { v->Normal[nrmIndex].x, v->Normal[nrmIndex].y, v->Normal[nrmIndex].z, 0.0f};
                    break;
                }
                case EVertexAttribute::TANGENT:
                case EVertexAttribute::TANGENT1:
                case EVertexAttribute::TANGENT2:
                case EVertexAttribute::TANGENT3:
                {
                    uint32_t tgtIndex = atr - (uint32_t)EVertexAttribute::TANGENT;

                    data = { v->Tangent[tgtIndex].x, v->Tangent[tgtIndex].y, v->Tangent[tgtIndex].z, 0.0f };
                    break;
                }
                case EVertexAttribute::BINORMAL:
                case EVertexAttribute::BINORMAL1:
                case EVertexAttribute::BINORMAL2:
                case EVertexAttribute::BINORMAL3:
                {
                    uint32_t binIndex = atr - (uint32_t)EVertexAttribute::BINORMAL;

                    data = { v->Binormal[binIndex].x, v->Binormal[binIndex].y, v->Binormal[binIndex].z, 0.0f };
                    break;
                }
                case EVertexAttribute::BLENDWEIGHT:
                case EVertexAttribute::BLENDWEIGHT1:
                case EVertexAttribute::BLENDWEIGHT2:
                case EVertexAttribute::BLENDWEIGHT3:
                {
                    uint32_t bwtIndex = atr - (uint32_t)EVertexAttribute::BLENDWEIGHT;

                    data = { v->BlendWeight[bwtIndex].x, v->BlendWeight[bwtIndex].y, v->BlendWeight[bwtIndex].z, v->BlendWeight[bwtIndex].w };
                    break;
                }
                case EVertexAttribute::BLENDINDEX:
                case EVertexAttribute::BLENDINDEX1:
                case EVertexAttribute::BLENDINDEX2:
                case EVertexAttribute::BLENDINDEX3:
                {
                    uint32_t bidIndex = atr - (uint32_t)EVertexAttribute::BLENDINDEX;

                    data = { v->BlendIndex[bidIndex].x, v->BlendIndex[bidIndex].y, v->BlendIndex[bidIndex].z, v->BlendIndex[bidIndex].w };
                    break;
                }
                case EVertexAttribute::COLOR:
                case EVertexAttribute::COLOR1:
                case EVertexAttribute::COLOR2:
                case EVertexAttribute::COLOR3:
                {
                    uint32_t colIndex = atr - (uint32_t)EVertexAttribute::COLOR;

                    data = { v->Color[colIndex].x, v->Color[colIndex].y, v->Color[colIndex].z, v->Color[colIndex].w };
                    break;
                }
                case EVertexAttribute::TEXCOORD:
                case EVertexAttribute::TEXCOORD1:
                case EVertexAttribute::TEXCOORD2:
                case EVertexAttribute::TEXCOORD3:
                case EVertexAttribute::TEXCOORD4:
                case EVertexAttribute::TEXCOORD5:
                case EVertexAttribute::TEXCOORD6:
                case EVertexAttribute::TEXCOORD7:
                case EVertexAttribute::TEXCOORD8:
                case EVertexAttribute::TEXCOORD9:
                case EVertexAttribute::TEXCOORD10:
                case EVertexAttribute::TEXCOORD11:
                case EVertexAttribute::TEXCOORD12:
                case EVertexAttribute::TEXCOORD13:
                case EVertexAttribute::TEXCOORD14:
                case EVertexAttribute::TEXCOORD15:
                case EVertexAttribute::TEXCOORD16:
                case EVertexAttribute::TEXCOORD17:
                case EVertexAttribute::TEXCOORD18:
                case EVertexAttribute::TEXCOORD19:
                case EVertexAttribute::TEXCOORD20:
                case EVertexAttribute::TEXCOORD21:
                case EVertexAttribute::TEXCOORD22:
                case EVertexAttribute::TEXCOORD23:
                {
                    uint32_t txcIndex = atr - (uint32_t)EVertexAttribute::TEXCOORD;

                    data = { v->TexCoord[txcIndex].x, v->TexCoord[txcIndex].y, 0.0f, 0.0f };
                    break;
                }
            }

            WriteVertexComponent(&stream, formats[atr], data);
        }
    }
}

uint16_t CVertexBuffer::CalculateVertexStride(std::vector<EVertexFormat> formats) {
    uint16_t stride = 0;

    for (EVertexFormat f : formats) {
        switch (f) {
            case EVertexFormat::RGBA32_FLOAT:
                stride += sizeof(float) * 4;
                break;
            case EVertexFormat::RGB32_FLOAT:
                stride += sizeof(float) * 3;
                break;
            case EVertexFormat::RG32:
                stride += sizeof(float) * 2;
                break;
            case EVertexFormat::RGBA16_FLOAT:
                stride += sizeof(uint16_t) * 4;
                break;
            case EVertexFormat::RG16_FLOAT:
                stride += sizeof(uint16_t) * 2;
                break;
            case EVertexFormat::RGB10A2:
            case EVertexFormat::RGBA8_UNORM:
            case EVertexFormat::RGBA8_UINT:
                stride += sizeof(uint32_t);
                break;
            case EVertexFormat::NONE:
            default:
                break;
        }
    }

    return stride;
}

uint32_t CVertexBuffer::GetAttributeCount(uint32_t startingIndex, uint32_t max) {
    uint32_t count = 0;
    
    for (int i = 0; i < max; i++) {
        if (mVertexFormat.mSizes[startingIndex + i] != 0) {
            count++;
        }
    }

    return count;
}


/* CIndexBuffer */

CIndexBuffer::CIndexBuffer() : mVTable(0), mIndexCount(0), mIndexStride(0), m000E(0), mFlags(0), m0014(0),
    mIndexData(nullptr), m0020(0), m0028(0), mShaderResourceView(0), m0038(0), m0040(0), m0048(0)
{

}

CIndexBuffer::~CIndexBuffer() {
    if (mIndexData != nullptr) {
        delete[] mIndexData;
        mIndexData = nullptr;
    }
}

void CIndexBuffer::Deserialize(bStream::CStream* stream) {
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

void CIndexBuffer::Serialize(bStream::CStream* stream) {

}

std::vector<uint32_t> CIndexBuffer::GetIndices() {
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
