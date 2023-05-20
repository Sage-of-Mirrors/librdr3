#pragma once
#include <iostream>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <cassert>

namespace bStream {

uint64_t swap64(uint64_t v);
uint32_t swap32(uint32_t v);
uint16_t swap16(uint16_t v);

template < typename T >
static inline const T * OffsetPointer(const void * ptr, size_t offs) {
  uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
  p += offs;
  return reinterpret_cast<const T *>(p);
}

template < typename T >
static inline T * OffsetWritePointer(void * ptr, size_t offs) {
  uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
  p += offs;
  return reinterpret_cast<T *>(p);
}

enum OpenMode {
	In,
	Out
};

enum Endianess{
	Big, Little
};

Endianess getSystemEndianess();

class CStream {
	public:
		virtual bool seek(size_t, bool = false) = 0;
		virtual void skip(size_t) = 0;
		virtual size_t tell() = 0;

		virtual uint8_t readUInt8() = 0;
		virtual uint16_t readUInt16() = 0;
		virtual uint32_t readUInt32() = 0;
		virtual uint64_t readUInt64() = 0;

		virtual int8_t readInt8() = 0;
		virtual int16_t readInt16() = 0;
		virtual int32_t readInt32() = 0;
		virtual int64_t readInt64() = 0;

		virtual float readFloat() = 0;
		virtual void writeFloat(float) = 0;

		virtual int8_t peekInt8(size_t) = 0;
		virtual uint8_t peekUInt8(size_t) = 0;

		virtual int16_t peekInt16(size_t) = 0;
		virtual uint16_t peekUInt16(size_t) = 0;

		virtual int32_t peekInt32(size_t) = 0;
		virtual uint32_t peekUInt32(size_t) = 0;

		virtual int64_t peekInt64(size_t) = 0;
		virtual uint64_t peekUInt64(size_t) = 0;

		virtual void writeInt8(int8_t) = 0;
		virtual void writeUInt8(uint8_t) = 0;

		virtual void writeInt32(int32_t) = 0;
		virtual void writeUInt32(uint32_t) = 0;
		
		virtual void writeInt16(int16_t) = 0;
		virtual void writeUInt16(uint16_t) = 0;

		virtual void readBytesTo(uint8_t*, size_t) = 0;
		virtual void writeBytesTo(uint8_t*, size_t) = 0;

		virtual void writeString(std::string) = 0;
		virtual std::string peekString(size_t, size_t) = 0;
		virtual std::string readString(size_t) = 0;
};

class CFileStream : public CStream {
private:
	std::fstream base;
	std::string filePath;
	OpenMode mode;
	Endianess order;
	Endianess systemOrder;

public:

	template<typename T>
	T readStruct(){
		assert(mode == OpenMode::In);
		T out;
		base.read((char*)&out, sizeof(T));
		return out;
	}

	//read functions
	int8_t readInt8();
	uint8_t readUInt8();
	int16_t readInt16();
	uint16_t readUInt16();
	int32_t readInt32();
	uint32_t readUInt32();
	int64_t readInt64();
	uint64_t readUInt64();
	float readFloat();
	char* readBytes(size_t);
	std::string readWString(size_t);
	std::string readString(size_t);
	void readBytesTo(uint8_t*, size_t);

	//write functions
	void writeInt8(int8_t);
	void writeUInt8(uint8_t);
	void writeInt16(int16_t);
	void writeUInt16(uint16_t);
	void writeInt32(int32_t);
	void writeUInt32(uint32_t);
	void writeFloat(float);
	void writeBytesTo(uint8_t*, size_t);
	void writeString(std::string);

	//utility functions
	size_t getSize();
	size_t tell();
	bool seek(size_t, bool = false);
	void skip(size_t);
	std::string getPath();

	int8_t peekInt8(size_t);
	uint8_t peekUInt8(size_t);

	int16_t peekInt16(size_t);
	uint16_t peekUInt16(size_t);
	
	int32_t peekInt32(size_t);
	uint32_t peekUInt32(size_t);

	int64_t peekInt64(size_t);
	uint64_t peekUInt64(size_t);

	std::string peekString(size_t, size_t);

	std::fstream &getStream();

	CFileStream(std::string, Endianess, OpenMode mod = OpenMode::In);
	CFileStream(std::string, OpenMode mod = OpenMode::In);
	CFileStream() {}
	~CFileStream() {this->base.close();}
};

class CMemoryStream : public CStream {
	private:
		uint8_t* mBuffer;
		size_t mPosition;
		size_t mSize;
		size_t mCapacity;
		int8_t mHasInternalBuffer;

		OpenMode mOpenMode;
		Endianess order;
		Endianess systemOrder;
	
	public:
		bool Reserve(size_t);

		size_t getSize();
		size_t getCapacity();
		
		int8_t readInt8();
		uint8_t readUInt8();

		int16_t readInt16();
		uint16_t readUInt16();

		int32_t readInt32();
		uint32_t readUInt32();

		int64_t readInt64();
		uint64_t readUInt64();

		float readFloat();

		int8_t peekInt8(size_t);
		uint8_t peekUInt8(size_t);

		int16_t peekInt16(size_t);
		uint16_t peekUInt16(size_t);

		int32_t peekInt32(size_t);
		uint32_t peekUInt32(size_t);

		int64_t peekInt64(size_t);
		uint64_t peekUInt64(size_t);

		void writeInt8(int8_t);
		void writeUInt8(uint8_t);
		
		void writeInt16(int16_t);
		void writeUInt16(uint16_t);

		void writeInt32(int32_t);
		void writeUInt32(uint32_t);

		void writeFloat(float);
		void writeBytesTo(uint8_t*, size_t);
		void writeString(std::string);

		std::string readString(size_t);
		std::string peekString(size_t, size_t);
		void readBytesTo(uint8_t*, size_t);

		bool seek(size_t, bool = false);
		void skip(size_t);
		size_t tell();

		uint8_t* getBuffer();

		CMemoryStream(uint8_t*, size_t, Endianess, OpenMode);
		CMemoryStream(size_t, Endianess, OpenMode);
		CMemoryStream(){}
		~CMemoryStream(){
			if(mHasInternalBuffer){
				delete[] mBuffer;
			}
		}

};
}

#if defined(BSTREAM_IMPLEMENTATION)
namespace bStream {

uint64_t swap64(uint64_t r) {
	return (((r >> 56) & 0x00000000000000FF) |
		((r >> 40) & 0x000000000000FF00) |
		((r >> 24) & 0x0000000000FF0000) |
		((r >> 8) & 0x00000000FF000000) |
		((r << 8) & 0x000000FF00000000) |
		((r << 24) & 0x0000FF0000000000) |
		((r << 40) & 0x00FF000000000000) |
		((r << 56) & 0xFF00000000000000));
}

uint32_t swap32(uint32_t r){
	return ( ((r>>24)&0xFF) | ((r<<8) & 0xFF0000) | ((r>>8)&0xFF00) | ((r<<24)&0xFF000000));
}

uint16_t swap16(uint16_t r){
	return ( ((r<<8)&0xFF00) | ((r>>8)&0x00FF) );
}

Endianess getSystemEndianess(){
	union {
		uint32_t integer;
		uint8_t bytes[sizeof(uint32_t)];
	} check;
	check.integer = 0x01020304U;
	return (check.bytes[0] == 0x01 ? Endianess::Big : Endianess::Little);
}

CFileStream::CFileStream(std::string path, Endianess ord, OpenMode mod){
	base.open(path, (mod == OpenMode::In ? std::ios::in : std::ios::out) | std::ios::binary);
	base.exceptions(std::ifstream::badbit);
	filePath = path;
	order = ord;
	mode = mod;
	systemOrder = getSystemEndianess();
}

CFileStream::CFileStream(std::string path, OpenMode mod){
	base.open(path, (mod == OpenMode::In ? std::ios::in : std::ios::out) | std::ios::binary);
	base.exceptions(std::ifstream::badbit);
	filePath = path;
	mode = mod;
	systemOrder = getSystemEndianess();
	order = getSystemEndianess();
}

std::fstream &CFileStream::getStream(){
	return base;
}

std::string CFileStream::getPath(){
	return filePath;
}

bool CFileStream::seek(size_t pos, bool fromCurrent){
	try {
		base.seekg(pos, (fromCurrent ? base.cur : base.beg));
		return true;
	} catch (const std::ifstream::failure& f) {
		return false;
	}
}

void CFileStream::skip(size_t amount){
	base.seekg(amount, base.cur);
}

size_t CFileStream::tell(){
	return base.tellg();
}

uint64_t CFileStream::readUInt64() {
	assert(mode == OpenMode::In);
	uint64_t r;
	base.read((char*)&r, sizeof(uint64_t));
	if (order != systemOrder) {
		return swap64(r);
	}
	else {
		return r;
	}
}

int64_t CFileStream::readInt64() {
	assert(mode == OpenMode::In);
	int64_t r;
	base.read((char*)&r, sizeof(int64_t));
	if (order != systemOrder) {
		return swap64(r);
	}
	else {
		return r;
	}
}

uint32_t CFileStream::readUInt32(){
	assert(mode == OpenMode::In);
	uint32_t r;
	base.read((char*)&r, sizeof(uint32_t));
	if(order != systemOrder){
		return swap32(r);
	}
	else{
		return r;
	}
}

int32_t CFileStream::readInt32(){
	assert(mode == OpenMode::In);
	int32_t r;
	base.read((char*)&r, sizeof(int32_t));
	if(order != systemOrder){
		return swap32(r);
	}
	else{
		return r;
	}
}

uint16_t CFileStream::readUInt16(){
	assert(mode == OpenMode::In);
	uint16_t r;
	base.read((char*)&r, sizeof(uint16_t));
	if(order != systemOrder){
		return swap16(r);
	}
	else{
		return r;
	}
}

int16_t CFileStream::readInt16(){
	assert(mode == OpenMode::In);
	int16_t r;
	base.read((char*)&r, sizeof(int16_t));
	if(order != systemOrder){
		return swap16(r);
	}
	else{
		return r;
	}
}

uint8_t CFileStream::readUInt8(){
	assert(mode == OpenMode::In);
	uint8_t r;
	base.read((char*)&r, sizeof(uint8_t));
	return r;
}

int8_t CFileStream::readInt8(){
	assert(mode == OpenMode::In);
	int8_t r;
	base.read((char*)&r, sizeof(int8_t));
	return r;
}

float CFileStream::readFloat(){
	assert(mode == OpenMode::In);
	char buff[sizeof(float)];
	base.read(buff, sizeof(float));
	if(order != systemOrder){
		char temp[sizeof(float)];
		temp[0] = buff[3];
		temp[1] = buff[2];
		temp[2] = buff[1];
		temp[3] = buff[0];
		return *((float*)temp);
	}
	return *((float*)buff);
}

char* CFileStream::readBytes(size_t size){
	assert(mode == OpenMode::In);
	char* buffer = new char[size];
	base.read(buffer, size);
	return buffer;
}

void CFileStream::readBytesTo(uint8_t* out_buffer, size_t len){
	assert(mode == OpenMode::In);
	base.read((char*)out_buffer, len);
}

std::string CFileStream::readString(size_t len){
	assert(mode == OpenMode::In);
    std::string str(len, '\0'); //creates string str at size of length and fills it with '\0'
    base.read(&str[0], len);
    return str;
}

std::string CFileStream::peekString(size_t at, size_t len){
	assert(mode == OpenMode::In);
    std::string str(len, '\0'); //creates string str at size of length and fills it with '\0'
	size_t cur = base.tellg();
	base.seekg(at, base.beg);
    base.read(&str[0], len);
	base.seekg(cur, base.beg);
    return str;
}

std::string CFileStream::readWString(size_t len){
	assert(mode == OpenMode::In);
    std::string str(len, '\0'); //creates string str at size of length and fills it with '\0'
    base.read(&str[0], len);
    return str;
}

void CFileStream::writeInt8(int8_t v){
	assert(mode == OpenMode::Out);
	base.write((char*)&v, 1);
}

void CFileStream::writeUInt8(uint8_t v){
	assert(mode == OpenMode::Out);
	base.write((char*)&v, 1);
}

void CFileStream::writeInt16(int16_t v){
	assert(mode == OpenMode::Out);
	if(order != systemOrder){
		v = swap16(v);
	}
	base.write((char*)&v, sizeof(uint16_t));
}

void CFileStream::writeUInt16(uint16_t v){
	assert(mode == OpenMode::Out);
	if(order != systemOrder){
		v = swap16(v);
	}
	base.write((char*)&v, sizeof(uint16_t));
}

void CFileStream::writeInt32(int32_t v){
	assert(mode == OpenMode::Out);
	if(order != systemOrder){
	   v = swap32(v);
	}
	base.write((char*)&v, sizeof(int32_t));
}

void CFileStream::writeUInt32(uint32_t v){
	assert(mode == OpenMode::Out);
	if(order != systemOrder){
	   v = swap32(v);
	}
	base.write((char*)&v, sizeof(uint32_t));
}

void CFileStream::writeFloat(float v){
	assert(mode == OpenMode::Out);
	char* buff = (char*)&v;
	if(order != systemOrder){
		char temp[sizeof(float)];
		temp[0] = buff[3];
		temp[1] = buff[2];
		temp[2] = buff[1];
		temp[3] = buff[0];
		v = *((float*)temp);
	}
	base.write((char*)&v, sizeof(float));
}

void CFileStream::writeString(std::string v){
	assert(mode == OpenMode::Out);
	base.write(v.c_str(), v.size());
}

void CFileStream::writeBytesTo(uint8_t* v, size_t size){
	assert(mode == OpenMode::Out);
	base.write(reinterpret_cast<char*>(v), size);
}

uint8_t CFileStream::peekUInt8(size_t offset){
	assert(mode == OpenMode::In);
	uint8_t ret;
	int pos = (int)base.tellg();
	base.seekg(offset, base.beg);
	ret = readUInt8();
	base.seekg(pos, base.beg);
	return ret;
}

int8_t CFileStream::peekInt8(size_t offset){
	assert(mode == OpenMode::In);
	int8_t ret;
	int pos = (int)base.tellg();
	base.seekg(offset, base.beg);
	ret = readInt8();
	base.seekg(pos, base.beg);
	return ret;
}

uint16_t CFileStream::peekUInt16(size_t offset){
	assert(mode == OpenMode::In);
	uint16_t ret;
	int pos = (int)base.tellg();
	base.seekg(offset, base.beg);
	ret = readUInt16();
	base.seekg(pos, base.beg);
	return ret;
}

int16_t CFileStream::peekInt16(size_t offset){
	assert(mode == OpenMode::In);
	int16_t ret;
	int pos = (int)base.tellg();
	base.seekg(offset, base.beg);
	ret = readInt16();
	base.seekg(pos, base.beg);
	return ret;
}

uint32_t CFileStream::peekUInt32(size_t offset){
	assert(mode == OpenMode::In);
	uint32_t ret;
	int pos = (int)base.tellg();
	base.seekg(offset, base.beg);
	ret = readUInt32();
	base.seekg(pos, base.beg);
	return ret;
}

int32_t CFileStream::peekInt32(size_t offset){
	assert(mode == OpenMode::In);
	int32_t ret;
	int pos = (int)base.tellg();
	base.seekg(offset, base.beg);
	ret = readInt32();
	base.seekg(pos, base.beg);
	return ret;
}

uint64_t CFileStream::peekUInt64(size_t offset) {
	assert(mode == OpenMode::In);
	uint64_t ret;
	int pos = (int)base.tellg();
	base.seekg(offset, base.beg);
	ret = readUInt64();
	base.seekg(pos, base.beg);
	return ret;
}

int64_t CFileStream::peekInt64(size_t offset) {
	assert(mode == OpenMode::In);
	int64_t ret;
	int pos = (int)base.tellg();
	base.seekg(offset, base.beg);
	ret = readInt64();
	base.seekg(pos, base.beg);
	return ret;
}

size_t CFileStream::getSize(){
	int pos = (int)base.tellg();
	base.seekg(0, std::ios::end);
	size_t ret = base.tellg();
	base.seekg(pos, std::ios::beg);
	return ret;
}

///
///
///  CMemoryStream
///
///


CMemoryStream::CMemoryStream(uint8_t* ptr, size_t size, Endianess ord, OpenMode mode){
	mBuffer = ptr;
	mPosition = 0;
	mSize = size;
	mCapacity = size;
	mHasInternalBuffer = false;
	mOpenMode = mode; 
	order = ord;
	systemOrder = getSystemEndianess();
}

CMemoryStream::CMemoryStream(size_t size, Endianess ord, OpenMode mode){
	mBuffer = new uint8_t[size]{};
	mPosition = 0;
	mSize = size;
	mCapacity = size;
	mHasInternalBuffer = true;
	mOpenMode = mode;
	order = ord;
	systemOrder = getSystemEndianess();
}

size_t CMemoryStream::getSize(){
	return mSize;
}

size_t CMemoryStream::getCapacity(){
	return mCapacity;
}

bool CMemoryStream::seek(size_t pos, bool fromCurrent){
	if(fromCurrent && mPosition + pos > mCapacity || pos > mCapacity) return false; 
	
	if(fromCurrent){
		mPosition += pos;
	} else {
		mPosition = pos;
	}
	
	if(mPosition > mSize) mSize = mPosition;
	
	return true;
}

void CMemoryStream::skip(size_t amount){
	mPosition += (mPosition + amount < mSize ? amount : 0);
}

size_t CMemoryStream::tell(){
	return mPosition;
}

uint8_t* CMemoryStream::getBuffer(){
	return mBuffer;
}

///
/// Memstream Reading Functions
///

int8_t CMemoryStream::readInt8(){
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	int8_t r;
	memcpy(&r, OffsetPointer<int8_t>(mBuffer, mPosition), sizeof(int8_t));
	mPosition++;
	return r;
}

uint8_t CMemoryStream::readUInt8(){
	assert(mOpenMode == OpenMode::In);
	uint8_t r;
	memcpy(&r, OffsetPointer<uint8_t>(mBuffer, mPosition), sizeof(uint8_t));
	mPosition++;
	return r;
}

int16_t CMemoryStream::readInt16(){
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	int16_t r;
	memcpy(&r, OffsetPointer<int16_t>(mBuffer, mPosition), sizeof(int16_t));
	mPosition += sizeof(int16_t);

	if(order != systemOrder){
		return swap16(r);
	}
	else{
		return r;
	}
}

uint16_t CMemoryStream::readUInt16(){
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	uint16_t r;
	memcpy(&r, OffsetPointer<uint16_t>(mBuffer, mPosition), sizeof(uint16_t));
	mPosition += sizeof(uint16_t);

	if(order != systemOrder){
		return swap16(r);
	}
	else{
		return r;
	}
}

uint32_t CMemoryStream::readUInt32(){
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	uint32_t r;
	memcpy(&r, OffsetPointer<uint32_t>(mBuffer, mPosition), sizeof(uint32_t));
	mPosition += sizeof(uint32_t);

	if(order != systemOrder){
		return swap32(r);
	}
	else{
		return r;
	}
}

int32_t CMemoryStream::readInt32(){
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	int32_t r;
	memcpy(&r, OffsetPointer<int32_t>(mBuffer, mPosition), sizeof(int32_t));
	mPosition += sizeof(int32_t);

	if(order != systemOrder){
		return swap32(r);
	}
	else{
		return r;
	}
}

uint64_t CMemoryStream::readUInt64() {
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	uint64_t r;
	memcpy(&r, OffsetPointer<uint64_t>(mBuffer, mPosition), sizeof(uint64_t));
	mPosition += sizeof(uint64_t);

	if (order != systemOrder) {
		return swap64(r);
	}
	else {
		return r;
	}
}

int64_t CMemoryStream::readInt64() {
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	int64_t r;
	memcpy(&r, OffsetPointer<int64_t>(mBuffer, mPosition), sizeof(int64_t));
	mPosition += sizeof(int64_t);

	if (order != systemOrder) {
		return swap64(r);
	}
	else {
		return r;
	}
}


float CMemoryStream::readFloat(){
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	
	char buff[sizeof(float)];
	memcpy(&buff, OffsetPointer<int32_t>(mBuffer, mPosition), sizeof(float));
	mPosition += sizeof(float);
	if(order != systemOrder){
		char temp[sizeof(float)];
		temp[0] = buff[3];
		temp[1] = buff[2];
		temp[2] = buff[1];
		temp[3] = buff[0];
		return *((float*)temp);
	}

	else{
		return *((float*)buff);
	}
}

///
/// Memstream Peek Functions
///

int8_t CMemoryStream::peekInt8(size_t at){
	assert(mOpenMode == OpenMode::In && at < mSize);
	int8_t r;
	memcpy(&r, OffsetPointer<int8_t>(mBuffer, at), sizeof(int8_t));
	return r;
}

uint8_t CMemoryStream::peekUInt8(size_t at){
	assert(mOpenMode == OpenMode::In && at < mSize);
	uint8_t r;
	memcpy(&r, OffsetPointer<uint8_t>(mBuffer, at), sizeof(uint8_t));
	return r;
}

int16_t CMemoryStream::peekInt16(size_t at){
	assert(mOpenMode == OpenMode::In && at < mSize);
	int16_t r;
	memcpy(&r, OffsetPointer<int16_t>(mBuffer, at), sizeof(int16_t));

	if(order != systemOrder){
		return swap16(r);
	}
	else{
		return r;
	}
}

uint16_t CMemoryStream::peekUInt16(size_t at){
	assert(mOpenMode == OpenMode::In && at < mSize);
	uint16_t r;
	memcpy(&r, OffsetPointer<uint16_t>(mBuffer, at), sizeof(uint16_t));

	if(order != systemOrder){
		return swap16(r);
	}
	else{
		return r;
	}
}

uint32_t CMemoryStream::peekUInt32(size_t at){
	assert(mOpenMode == OpenMode::In && at < mSize);
	uint32_t r;
	memcpy(&r, OffsetPointer<uint32_t>(mBuffer, at), sizeof(uint32_t));

	if(order != systemOrder){
		return swap32(r);
	}
	else{
		return r;
	}
}

int32_t CMemoryStream::peekInt32(size_t at){
	assert(mOpenMode == OpenMode::In && at < mSize);
	int32_t r;
	memcpy(&r, OffsetPointer<int32_t>(mBuffer, at), sizeof(int32_t));

	if(order != systemOrder){
		return swap32(r);
	}
	else{
		return r;
	}
}

uint64_t CMemoryStream::peekUInt64(size_t at) {
	assert(mOpenMode == OpenMode::In && at < mSize);
	uint64_t r;
	memcpy(&r, OffsetPointer<uint64_t>(mBuffer, at), sizeof(uint64_t));

	if (order != systemOrder) {
		return swap64(r);
	}
	else {
		return r;
	}
}

int64_t CMemoryStream::peekInt64(size_t at) {
	assert(mOpenMode == OpenMode::In && at < mSize);
	int64_t r;
	memcpy(&r, OffsetPointer<int64_t>(mBuffer, at), sizeof(int64_t));

	if (order != systemOrder) {
		return swap64(r);
	}
	else {
		return r;
	}
}

std::string CMemoryStream::readString(size_t len){
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	std::string str(OffsetPointer<char>(mBuffer, mPosition),OffsetPointer<char>(mBuffer, mPosition+len));
	mPosition += len;
	return str;
}

std::string CMemoryStream::peekString(size_t at, size_t len){
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	std::string str(OffsetPointer<char>(mBuffer, at), OffsetPointer<char>(mBuffer, at+len));
	return str;
}

//I don't like this set up, but for now it works
void CMemoryStream::readBytesTo(uint8_t* out_buffer, size_t len){
	assert(mOpenMode == OpenMode::In && mPosition < mSize);
	if(mPosition + len < mSize){
		memcpy(out_buffer, OffsetPointer<char>(mBuffer, mPosition), len);

		mPosition += len;
	}
}

///
/// Memstream Writing Functions
///

//included in writing functions because this is needed when using an internal buffer
bool CMemoryStream::Reserve(size_t needed){
	if(mCapacity >= needed){
		return true;
	}
	if(!mHasInternalBuffer){
		return false;
	}

	mCapacity *= 2;
	uint8_t* temp = new uint8_t[mCapacity]{};
	memcpy(temp, mBuffer, mSize);
	delete[] mBuffer;
	mBuffer = temp;

	mSize += needed;
	return true;
}

void CMemoryStream::writeInt8(int8_t v){
	Reserve(mPosition + sizeof(v));
	memcpy(OffsetWritePointer<int8_t>(mBuffer, mPosition), &v, sizeof(int8_t));
	mPosition += sizeof(int8_t);
}

void CMemoryStream::writeUInt8(uint8_t v){
	Reserve(mPosition + sizeof(v));
	memcpy(OffsetWritePointer<uint8_t>(mBuffer, mPosition), &v, sizeof(int8_t));
	mPosition += sizeof(int8_t);
}

void CMemoryStream::writeInt16(int16_t v){
	Reserve(mPosition + sizeof(v));

	if (order != systemOrder)
		v = swap16(v);

	memcpy(OffsetWritePointer<int16_t>(mBuffer, mPosition), &v, sizeof(int16_t));
	mPosition += sizeof(int16_t);
}

void CMemoryStream::writeUInt16(uint16_t v){
	Reserve(mPosition + sizeof(v));

	if (order != systemOrder)
		v = swap16(v);

	memcpy(OffsetWritePointer<uint16_t>(mBuffer, mPosition), &v, sizeof(int16_t));
	mPosition += sizeof(int16_t);
}

void CMemoryStream::writeInt32(int32_t v){
	Reserve(mPosition + sizeof(v));

	if (order != systemOrder)
		v = swap32(v);

	memcpy(OffsetWritePointer<int32_t>(mBuffer, mPosition), &v, sizeof(int32_t));
	mPosition += sizeof(int32_t);
}

void CMemoryStream::writeUInt32(uint32_t v){
	Reserve(mPosition + sizeof(v));

	if (order != systemOrder)
		v = swap32(v);

	memcpy(OffsetWritePointer<uint32_t>(mBuffer, mPosition), &v, sizeof(int32_t));
	mPosition += sizeof(int32_t);
}

void CMemoryStream::writeFloat(float v){
	Reserve(mPosition + sizeof(v));

	if (order != systemOrder)
		v = swap32(v);

	memcpy(OffsetWritePointer<float>(mBuffer, mPosition), &v, sizeof(float));
	mPosition += sizeof(float);
}


//TODO: Clean these up and test them more

void CMemoryStream::writeBytesTo(uint8_t* bytes, size_t size){
	Reserve(mPosition + size);
	memcpy(OffsetWritePointer<uint8_t>(mBuffer, mPosition), bytes, size);
	mPosition += size;
}

void CMemoryStream::writeString(std::string str){
	Reserve(mPosition + str.size());
	memcpy(OffsetWritePointer<char>(mBuffer, mPosition), str.data(), str.size());
	mPosition += str.size();
}

}
#endif
