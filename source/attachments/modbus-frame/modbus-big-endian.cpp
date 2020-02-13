#include <cassert>
#include <iostream>

// Modbus帧数据字节序转换 大端

template<typename T>
static inline T bufferToU16(const void* data)
{
	auto ptr = reinterpret_cast<const unsigned char*>(data);
	// u16 -> buff
	// AB  -> AB
	return /*A*/ (*(ptr) << 8) | /*B*/ (*(ptr + 1));
}

template<typename T>
static inline T bufferToU32(const void* data)
{
	auto ptr = reinterpret_cast<const unsigned char*>(data);
	// u32  -> buff
	// ABCD -> ABCD
	return /*A*/ (*(ptr) << 24) | /*B*/ (*(ptr + 1) << 16) | /*C*/ (*(ptr + 2) << 8) | /*D*/ (*(ptr + 3));
}

template<typename T>
static inline void u16ToBuffer(T input, void* output)
{
	auto ptr   = reinterpret_cast<unsigned char*>(output);
	// buff -> u16
	// AB   -> AB
	*(ptr++) = /*A*/ (input >> 8) & 0xff;
	*(ptr)   = /*B*/ input & 0xff;
}

template<typename T>
static inline void u32ToBuffer(T input, void* output)
{
	auto ptr   = reinterpret_cast<unsigned char*>(output);
	// buff -> u32
	// ABCD -> ABCD
	*(ptr++) = /*A*/ (input >> 24) & 0xff;
	*(ptr++) = /*B*/ (input >> 16) & 0xff;
	*(ptr++) = /*C*/ (input >> 8) & 0xff;
	*(ptr)   = /*D*/ input & 0xff;
}

int main()
{
	unsigned char buf[100] = {0};

	// 16bit
	const unsigned short u16 = 0x1234;
	u16ToBuffer(u16, buf);
	assert(0x12 == buf[0]);
	assert(0x34 == buf[1]);
	assert(bufferToU16<unsigned short>(buf) == u16);

	// 32bit
	const unsigned int u32 = 0x12345678;
	u32ToBuffer(u32, buf);
	assert(0x12 == buf[0]);
	assert(0x34 == buf[1]);
	assert(0x56 == buf[2]);
	assert(0x78 == buf[3]);
	assert(bufferToU32<unsigned int>(buf) == u32);

	std::cout << "OK!" << std::endl;
	return 0;
}
