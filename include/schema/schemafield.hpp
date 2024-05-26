#pragma once

#include "types.h"

namespace rdr3 {
	namespace schema {
		enum class ESchemaFieldType : uint8_t;

		class CSchemaField {
			uint32_t mNameHash;        // 0x00
			uint32_t mOffset;          // 0x04
			ESchemaFieldType mType;    // 0x08
			uint8_t mAuxFieldIndex;    // 0x09
			uint16_t mRefTypeIndex;    // 0x0A
			uint32_t mAuxTypeNameHash; // 0x0C

		public:
			CSchemaField();
			virtual ~CSchemaField();

			uint32_t GetNameHash() const { return mNameHash; }
			uint32_t GetOffset() const { return mOffset; }
			ESchemaFieldType GetDataType() const { return mType; }
			uint16_t GetAuxFieldIndex() const { return mAuxFieldIndex; }
			uint32_t GetAuxTypeNameHash() const { return mAuxTypeNameHash; }
			
			bool PsoDeserialize(bStream::CStream* stream);
		};

		enum class ESchemaFieldType : uint8_t {
			FTP_NONE = 0x00,

			FTP_BOOL = 0x01,

			FTP_STRUCT = 0x05,

			FTP_STRUCTPTR = 0x07,
			FTP_GUID = 0x08,

			FTP_S8 = 0x10,
			FTP_U8 = 0x11,
			FTP_S16 = 0x12,
			FTP_U16 = 0x13,
			FTP_S32 = 0x14,
			FTP_U32 = 0x15,
			FTP_S64 = 0x16,
			FTP_U64 = 0x17,

			FTP_F16 = 0x20,
			FTP_F32 = 0x21,
			FTP_F64 = 0x22,

			FTP_VEC3V = 0x31,

			FTP_F32_XYZ = 0x33,
			FTP_F32_XYZW = 0x34,

			FTP_MAT34V = 0x36,

			FTP_CHARARRAY = 0x40,

			FTP_CHARPTR = 0x44,

			FTP_HASH = 0x4A,

			FTP_BYTEARRAY = 0x50,

			FTP_ARRAY = 0x52,
			FTP_FIXEDARRAY = 0x53,

			FTP_DATABLOCKPTR = 0x59,

			FTP_ENUM8 = 0x60,
			FTP_ENUM16 = 0x61,
			FTP_ENUM32 = 0x62,
			FTP_FLAGS32_1 = 0x63,
			FTP_FLAGS16 = 0x64,
			FTP_FLAGS32_2 = 0x65,

			FTP_BINARYMAP = 0x71
		};
	}
}
