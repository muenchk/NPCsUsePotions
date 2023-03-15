#include "BufferOperations.h"

namespace Buffer
{
	void Write(uint32_t value, unsigned char* buffer, int& offset)
	{
		uint32_t* ptr = (uint32_t*)(buffer + offset);
		*ptr = value;
		offset += 4;  // size written
	}

	void Write(uint64_t value, unsigned char* buffer, int& offset)
	{
		uint64_t* ptr = (uint64_t*)(buffer + offset);
		*ptr = value;
		offset += 8;  // size written
	}

	void Write(bool value, unsigned char* buffer, int& offset)
	{
		unsigned char* ptr = (unsigned char*)(buffer + offset);
		*ptr = (unsigned char)value;
		offset += 1;  // size written
	}

	void Write(int32_t value, unsigned char* buffer, int& offset)
	{
		int32_t* ptr = (int32_t*)(buffer + offset);
		*ptr = value;
		offset += 4;  // size written
	}

	void Write(int64_t value, unsigned char* buffer, int& offset)
	{
		int64_t* ptr = (int64_t*)(buffer + offset);
		*ptr = value;
		offset += 8;  // size written
	}

	void Write(float value, unsigned char* buffer, int& offset)
	{
		float* ptr = (float*)(buffer + offset);
		*ptr = value;
		offset += 4;  // size written
	}

	void Write(std::string value, unsigned char* buffer, int& offset)
	{
		size_t length = value.length();
		*((size_t*)(buffer + offset)) = length;
		offset += 8;
		if (length != 0) {
			std::strncpy((char*)(buffer + offset), value.c_str(), length);
			offset += (int)length;
		}
	}

	uint32_t ReadUInt32(unsigned char* buffer, int& offset)
	{
		offset += 4;  // size read
		return *((uint32_t*)(buffer + offset - 4));
	}

	uint64_t ReadUInt64(unsigned char* buffer, int& offset)
	{
		offset += 8;  // size read
		return *((uint64_t*)(buffer + offset - 8));
	}

	bool ReadBool(unsigned char* buffer, int& offset)
	{
		offset += 1;  // size read
		return (bool)(*((unsigned char*)(buffer + offset - 1)));
	}

	int32_t ReadInt32(unsigned char* buffer, int& offset)
	{
		offset += 4;  // size read
		return *((int32_t*)(buffer + offset - 4));
	}

	int64_t ReadInt64(unsigned char* buffer, int& offset)
	{
		offset += 8;  // size read
		return *((int64_t*)(buffer + offset - 8));
	}

	float ReadFloat(unsigned char* buffer, int& offset)
	{
		offset += 4;  // size read
		return *((float*)(buffer + offset - 4));
	}

	std::string ReadString(unsigned char* buffer, int& offset)
	{
		size_t length = *((size_t*)(buffer + offset));
		offset += 8;
		if (length != 0) {
			std::string tmp = std::string((char*)(buffer + offset), length);
			offset += (int)length;
			return tmp;
		} else
			return "";
	}

	int CalcStringLength(std::string value)
	{
		size_t length = value.length();
		length += 8;
		return (int)length;
	}

	size_t CalcStringLength(unsigned char* buffer, int offset)
	{
		size_t length = *((size_t*)(buffer + offset));
		return length;
	}
}
