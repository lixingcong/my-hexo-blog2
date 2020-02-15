#include <stdio.h>
#include <assert.h>

void u16ToBigEndian(unsigned short u16, unsigned char* pBuffer)
{
	*(pBuffer++) = u16 >> 8;
	*(pBuffer++) = u16 & 0xff;
}

unsigned short bigEndianToU16(const unsigned char* pBuffer)
{
	unsigned short num = 0;
	num |= (unsigned short) (*(pBuffer++)) << 8;
	num |= (unsigned short) (*(pBuffer++));
	return num;
}

int main()
{
	const unsigned short u16 = 0x1234;
	unsigned char        buffer[2];

	// u16 -> buffer
	u16ToBigEndian(u16, buffer);
	assert(0x12 == buffer[0]);
	assert(0x34 == buffer[1]);

	// buffer -> u16
	assert(u16 == bigEndianToU16(buffer));
	printf("OK!\n");
	
	return 0;
}
