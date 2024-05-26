#include "schema/schema.hpp"
#include "schema/schemafield.hpp"
#include "schema/schemadatablock.hpp"

#include "util/bstream.h"

namespace rdr3 {
	namespace schema {
		SchemaClassDB SchemaClassDatabase;
		SchemaEnumDB SchemaEnumDatabase;
	}
}

bool rdr3::schema::StoreSchemaClass(std::shared_ptr<CSchemaClass> schemaClass) {
	if (SchemaClassDatabase.find(schemaClass->GetNameHash()) == SchemaClassDatabase.end()) {
		SchemaClassDatabase.insert(std::pair< int32_t, std::shared_ptr<CSchemaClass>>(schemaClass->GetNameHash(), schemaClass));

		return true;
	}

	return false;
}

bool rdr3::schema::StoreSchemaEnum(std::shared_ptr<CSchemaEnum> schemaEnum) {
	if (SchemaEnumDatabase.find(schemaEnum->GetNameHash()) == SchemaEnumDatabase.end()) {
		SchemaEnumDatabase.insert(std::pair< int32_t, std::shared_ptr<CSchemaEnum>>(schemaEnum->GetNameHash(), schemaEnum));

		return true;
	}

	return false;
}

bool rdr3::schema::IsSchemaClassLoaded(uint32_t classNameHash) {
	return SchemaClassDatabase.find(classNameHash) != SchemaClassDatabase.end();
}

bool rdr3::schema::DoesSchemaClassHaveField(uint32_t classNameHash, uint32_t fieldNameHash) {
	if (!IsSchemaClassLoaded(classNameHash)) {
		return false;
	}

	std::shared_ptr<CSchemaClass> cls = SchemaClassDatabase[classNameHash];
	return cls->ContainsField(fieldNameHash);
}

bool rdr3::schema::GetBool(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<bool>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		false;
}

int8_t rdr3::schema::GetS8(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<int8_t>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		int8_t(0);
}

uint8_t rdr3::schema::GetU8(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<uint8_t>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		uint8_t(0);
}

int16_t rdr3::schema::GetS16(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<int16_t>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		int16_t(0);
}

uint16_t rdr3::schema::GetU16(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<uint16_t>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		uint16_t(0);
}

int32_t rdr3::schema::GetS32(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<int32_t>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		int32_t(0);
}

uint32_t rdr3::schema::GetU32(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<uint32_t>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		uint32_t(0);
}

int64_t rdr3::schema::GetS64(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<int16_t>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		int64_t(0);
}

uint64_t rdr3::schema::GetU64(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<uint64_t>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		uint64_t(0);
}

float rdr3::schema::GetF32(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<float>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		0.0f;
}

double rdr3::schema::GetF64(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	return DoesSchemaClassHaveField(classNameHash, fieldNameHash) ?
		data->GetDataAsIntegral<double>(SchemaClassDatabase[classNameHash]->GetField(fieldNameHash)->GetOffset() + offset) :
		0.0;
}

Vector3 rdr3::schema::GetVec3(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	Vector3 vec;
	if (!DoesSchemaClassHaveField(classNameHash, fieldNameHash)) {
		return vec;
	}

	std::shared_ptr<CSchemaField> vecField = SchemaClassDatabase[classNameHash]->GetField(fieldNameHash);
	vec.x = data->GetDataAsIntegral<float>(vecField->GetOffset() + offset);
	vec.y = data->GetDataAsIntegral<float>(vecField->GetOffset() + offset + sizeof(float));
	vec.z = data->GetDataAsIntegral<float>(vecField->GetOffset() + offset + sizeof(float) * 2);

	return vec;
}

Vector4 rdr3::schema::GetVec4(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	Vector4 vec;
	if (!DoesSchemaClassHaveField(classNameHash, fieldNameHash)) {
		return vec;
	}

	std::shared_ptr<CSchemaField> vecField = SchemaClassDatabase[classNameHash]->GetField(fieldNameHash);
	vec.x = data->GetDataAsIntegral<float>(vecField->GetOffset() + offset);
	vec.y = data->GetDataAsIntegral<float>(vecField->GetOffset() + offset + sizeof(float));
	vec.z = data->GetDataAsIntegral<float>(vecField->GetOffset() + offset + sizeof(float) * 2);
	vec.w = data->GetDataAsIntegral<float>(vecField->GetOffset() + offset + sizeof(float) * 3);

	return vec;
}

Matrix3x4 rdr3::schema::GetMat34(uint32_t classNameHash, uint32_t fieldNameHash, std::shared_ptr<CSchemaDataBlock> data, uint32_t offset) {
	Matrix3x4 mat;
	if (!DoesSchemaClassHaveField(classNameHash, fieldNameHash)) {
		return mat;
	}

	std::shared_ptr<CSchemaField> matField = SchemaClassDatabase[classNameHash]->GetField(fieldNameHash);

	mat.r0.x = data->GetDataAsIntegral<float>(matField->GetOffset() + offset);
	mat.r0.y = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float));
	mat.r0.z = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float) * 2);
	mat.r0.w = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float) * 3);

	mat.r1.x = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float) * 4);
	mat.r1.y = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float) * 5);
	mat.r1.z = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float) * 6);
	mat.r1.w = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float) * 7);

	mat.r2.x = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float) * 8);
	mat.r2.y = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float) * 9);
	mat.r2.z = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float) * 10);
	mat.r2.w = data->GetDataAsIntegral<float>(matField->GetOffset() + offset + sizeof(float) * 11);

	return mat;
}

bool rdr3::schema::SSchemaArrayInfo::Deserialize(bStream::CStream* stream) {
	BlockId = stream->readUInt8();
	Field0001 = stream->readUInt8();
	Field0002 = stream->readUInt8();
	Offset = stream->readUInt16() * 0x10;
	Field0005 = stream->readUInt8();
	Field0006 = stream->readUInt8();
	Field0007 = stream->readUInt8();

	return true;
}

std::vector<rdr3::schema::SSchemaArrayInfo> rdr3::schema::GetArrrayInfo(std::shared_ptr<CSchemaDataBlock> data) {
	std::vector<rdr3::schema::SSchemaArrayInfo> arrayInfo;

	uint32_t count = data->GetSize() / 0x08;
	bStream::CMemoryStream stream = data->GetDataAsStream(0, data->GetSize());

	for (uint32_t i = 0; i < count; i++) {
		SSchemaArrayInfo info;
		info.Deserialize(&stream);

		arrayInfo.push_back(info);
	}

	return arrayInfo;
}
