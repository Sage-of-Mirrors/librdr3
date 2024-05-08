#pragma once

#include "types.h"

#include "util/math.hpp"

#include <filesystem>

namespace rdr3 {
	namespace misc
	{
		struct CHeightmapHeader {
			uint32_t Magic;

			uint8_t Version;
			uint8_t ElementSize;
			uint32_t Flags;

			uint16_t ColCount;
			uint16_t RowCount;

			Vector3 MinBounds;
			Vector3 MaxBounds;

			uint32_t DataSize;
		};

		struct CHeightmapRow {
			uint16_t Field_0000;
			uint16_t ElementCount;
			uint32_t StartOffset;
		};

		class CHeightmap {
			CHeightmapHeader mHeader;

			shared_vector<Vector3> mPositions;
			shared_vector<Vector3> mNormals;
			std::vector<uint32_t> mIndices;

			void GenerateVertices(bStream::CStream* stream);
			void GenerateIndices();

		public:
			CHeightmap();

			bool Deserialize(bStream::CStream* stream);
			
			void Debug_ExportObj(std::filesystem::path filePath);
		};
	}
}
