#include "misc/heightmap.hpp"

#include "util/bstream.h"

#include <sstream>

constexpr uint32_t HEIGHTMAP_MAGIC = 0x484D4150;

rdr3::misc::CHeightmap::CHeightmap() {

}

uint64_t GetHeightMax(uint8_t elemSize) {
	switch (elemSize) {
		case 1:
		{
			return UINT8_MAX;
		}
		case 2:
		{
			return UINT16_MAX;
		}
		case 4:
		{
			return UINT32_MAX;
		}
		case 8:
		{
			return UINT64_MAX;
		}
	}

	return 0;
}

bool rdr3::misc::CHeightmap::Deserialize(bStream::CStream* stream) {
	if (stream->peekUInt32(0) != HEIGHTMAP_MAGIC) {
		return false;
	}

	mHeader.Magic = stream->readUInt32();
	mHeader.Version = stream->readUInt8();
	mHeader.ElementSize = stream->readUInt8();

	stream->skip(sizeof(uint16_t));

	mHeader.Flags = stream->readUInt32();
	mHeader.ColCount = stream->readUInt16();
	mHeader.RowCount = stream->readUInt16();
	mHeader.MinBounds = { stream->readFloat(), stream->readFloat(), stream->readFloat() };
	mHeader.MaxBounds = { stream->readFloat(), stream->readFloat(), stream->readFloat() };
	mHeader.DataSize = stream->readUInt32();

	GenerateVertices(stream);
	GenerateIndices();
}

void rdr3::misc::CHeightmap::GenerateVertices(bStream::CStream* stream) {
	Vector3 size = mHeader.MaxBounds - mHeader.MinBounds;

	float cellSizeX = (float)mHeader.ColCount / size.x;
	float cellSizeY = (float)mHeader.RowCount / size.y;

	size_t elementsOffset = sizeof(CHeightmapHeader) + (sizeof(CHeightmapRow) * mHeader.RowCount);
	size_t curStreamPos = stream->tell();

	for (uint32_t c = 0; c < mHeader.ColCount; c++) {
		CHeightmapRow row = { stream->readUInt16(), stream->readUInt16(), stream->readUInt32() };

		curStreamPos = stream->tell();
		stream->seek(elementsOffset + row.StartOffset);

		float h = 0.0f;
		for (uint16_t r = 0; r < row.ElementCount; r++) {
			switch (mHeader.ElementSize) {
				case 1:
				{
					h = (float)stream->readUInt8();
					break;
				}
				case 2:
				{
					h = (float)stream->readUInt16();
					break;
				}
				case 4:
				{
					h = (float)stream->readUInt32();
					break;
				}
				case 8:
				{
					h = (float)stream->readUInt64();
					break;
				}
			}

			float posX = (c / cellSizeX) + mHeader.MinBounds.x;
			float posY = (r / cellSizeY) + mHeader.MinBounds.y;
			float posZ = mHeader.MinBounds.z + (size.z * ((float)h / (float)GetHeightMax(mHeader.ElementSize)));

			std::shared_ptr<Vector3> pos = std::make_shared<Vector3>(posX, -posY, posZ);
			mPositions.push_back(pos);
		}

		stream->seek(curStreamPos);
	}
}

void rdr3::misc::CHeightmap::GenerateIndices() {
	for (uint32_t c = 0; c < mHeader.ColCount - 1; c++) {
		for (uint32_t v = 0; v < mHeader.ColCount - 1; v++) {
			uint32_t colIdx1 = (c * mHeader.ColCount) + v;
			uint32_t colIdx2 = colIdx1 + 1;
			
			uint32_t rowIdx1 = ((c + 1) * mHeader.ColCount) + v;
			uint32_t rowIdx2 = rowIdx1 + 1;

			mIndices.push_back(colIdx2);
			mIndices.push_back(colIdx1);
			mIndices.push_back(rowIdx1);

			mIndices.push_back(colIdx2);
			mIndices.push_back(rowIdx1);
			mIndices.push_back(rowIdx2);
		}
	}
}

void rdr3::misc::CHeightmap::Debug_ExportObj(std::filesystem::path filePath) {
	std::stringstream strm;

	for (std::shared_ptr<Vector3> v : mPositions) {
		strm << "v " << v->x << " " << v->y << " " << v->z << std::endl;
	}

	strm << std::endl;

	for (uint32_t i = 0; i < mIndices.size(); i += 6) {
		strm << "f " << mIndices[i] + 1 << " " << mIndices[i + 1] + 1 << " " << mIndices[i + 2] + 1 << std::endl;
		strm << "f " << mIndices[i + 3] + 1 << " " << mIndices[i + 4] + 1 << " " << mIndices[i + 5] + 1 << std::endl;
	}

	std::ofstream outStrm(filePath);
	outStrm << strm.str();
	outStrm.close();
}
