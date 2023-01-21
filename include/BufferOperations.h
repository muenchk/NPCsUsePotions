#pragma once
#include <utility>
#include <type_traits>


namespace Buffer
{
	/// <summary>
	/// Writes an UInt32 to the buffer
	/// </summary>
	/// <param name="value">value to write</param>
	/// <param name="buffer">buffer to write to</param>
	/// <param name="offset">offset to write at, is updated with the written size</param>
	void Write(uint32_t value, unsigned char* buffer, int &offset)
	{
		uint32_t* ptr = (uint32_t*)(buffer + offset);
		*ptr = value;
		offset += 4; // size written
	}
	/// <summary>
	/// Writes an UInt64 to the buffer
	/// </summary>
	/// <param name="value">value to write</param>
	/// <param name="buffer">buffer to write to</param>
	/// <param name="offset">offset to write at, is updated with the written size</param>
	void Write(uint64_t value, unsigned char* buffer, int& offset)
	{
		uint64_t* ptr = (uint64_t*)(buffer + offset);
		*ptr = value;
		offset += 8;  // size written
	}
	/// <summary>
	/// Writes a Boolean to the buffer
	/// </summary>
	/// <param name="value">value to write</param>
	/// <param name="buffer">buffer to write to</param>
	/// <param name="offset">offset to write at, is updated with the written size</param>
	void Write(bool value, unsigned char* buffer, int& offset)
	{
		unsigned char* ptr = (unsigned char*)(buffer + offset);
		*ptr = (unsigned char)value;
		offset += 1;  // size written
	}
	/// <summary>
	/// Writes an Int32 to the buffer
	/// </summary>
	/// <param name="value">value to write</param>
	/// <param name="buffer">buffer to write to</param>
	/// <param name="offset">offset to write at, is updated with the written size</param>
	void Write(int32_t value, unsigned char* buffer, int& offset)
	{
		int32_t* ptr = (int32_t*)(buffer + offset);
		*ptr = value;
		offset += 4;  // size written
	}
	/// <summary>
	/// Wrotes an Int64 to the buffer
	/// </summary>
	/// <param name="value">value to write</param>
	/// <param name="buffer">buffer to write to</param>
	/// <param name="offset">offset to write at, is updated with the written size</param>
	void Write(int64_t value, unsigned char* buffer, int& offset)
	{
		int64_t* ptr = (int64_t*)(buffer + offset);
		*ptr = value;
		offset += 8;  // size written
	}
	/// <summary>
	/// Writes a Float to the buffer
	/// </summary>
	/// <param name="value">value to write</param>
	/// <param name="buffer">buffer to write to</param>
	/// <param name="offset">offset to write at, is updated with the written size</param>
	void Write(float value, unsigned char* buffer, int& offset)
	{
		float* ptr = (float*)(buffer + offset);
		*ptr = value;
		offset += 4;  // size written
	}
	/// <summary>
	/// Writes a String to the buffer
	/// </summary>
	/// <param name="value">value to write</param>
	/// <param name="buffer">buffer to write to</param>
	/// <param name="offset">offset to write at, is updated with the written size</param>
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

	/// <summary>
	/// Reads an UInt32 from the buffer
	/// </summary>
	/// <param name="buffer">buffer to read from</param>
	/// <param name="offset">Offset to read at, is updated with the read size</param>
	/// <returns>the read value</returns>
	uint32_t ReadUInt32(unsigned char* buffer, int &offset)
	{
		offset += 4;  // size read
		return *((uint32_t*)(buffer + offset - 4));
	}
	/// <summary>
	/// Reads an UInt64 from the buffer
	/// </summary>
	/// <param name="buffer">buffer to read from</param>
	/// <param name="offset">Offset to read at, is updated with the read size</param>
	/// <returns>the read value</returns>
	uint64_t ReadUInt64(unsigned char* buffer, int &offset)
	{
		offset += 8;  // size read
		return *((uint64_t*)(buffer + offset - 8));
	}
	/// <summary>
	/// Reads a Boolean from the buffer
	/// </summary>
	/// <param name="buffer">buffer to read from</param>
	/// <param name="offset">Offset to read at, is updated with the read size</param>
	/// <returns>the read value</returns>
	bool ReadBool(unsigned char* buffer, int& offset)
	{
		offset += 1;  // size read
		return (bool)(*((unsigned char*)(buffer + offset - 1)));
	}
	/// <summary>
	/// Reads an Int32 from the buffer
	/// </summary>
	/// <param name="buffer">buffer to read from</param>
	/// <param name="offset">Offset to read at, is updated with the read size</param>
	/// <returns>the read value</returns>
	int32_t ReadInt32(unsigned char* buffer, int &offset)
	{
		offset += 4;  // size read
		return *((int32_t*)(buffer + offset - 4));
	}
	/// <summary>
	/// Reads an Int64 from the buffer
	/// </summary>
	/// <param name="buffer">buffer to read from</param>
	/// <param name="offset">Offset to read at, is updated with the read size</param>
	/// <returns>the read value</returns>
	int64_t ReadInt64(unsigned char* buffer, int &offset)
	{
		offset += 8;  // size read
		return *((int64_t*)(buffer + offset - 8));
	}
	/// <summary>
	/// Reads a Float from the buffer
	/// </summary>
	/// <param name="buffer">buffer to read from</param>
	/// <param name="offset">Offset to read at, is updated with the read size</param>
	/// <returns>the read value</returns>
	float ReadFloat(unsigned char* buffer, int &offset)
	{
		offset += 4;  // size read
		return *((float*)(buffer + offset - 4));
	}
	/// <summary>
	/// Reads a String from the buffer
	/// </summary>
	/// <param name="buffer">buffer to read from</param>
	/// <param name="offset">Offset to read at, is updated with the read size</param>
	/// <returns>the read value</returns>
	std::string ReadString(unsigned char* buffer, int &offset)
	{
		size_t length = *((size_t*)(buffer + offset));
		offset += 8;
		if (length != 0) {
			std::string tmp = std::string((char*)(buffer + offset), length);
			offset += (int)length;
			return tmp;
		}
		else
			return "";
	}

	/// <summary>
	/// Calculates the write length of a string
	/// </summary>
	/// <param name="value">the string to calculate for</param>
	/// <returns>The number of bytes needed to store the string</returns>
	int CalcStringLength(std::string value)
	{
		size_t length = value.length();
		length += 8;
		return (int)length;
	}
	/// <summary>
	/// Calcuates the read length of a string
	/// </summary>
	/// <param name="buffer">buffer to read from</param>
	/// <param name="offset">offset of the strings begin</param>
	/// <returns>The length of the string in bytes</returns>
	size_t CalcStringLength(unsigned char* buffer, int offset)
	{
		size_t length = *((size_t*)(buffer + offset));
		return length;
	}
}
