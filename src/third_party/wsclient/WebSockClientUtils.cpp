#ifndef WEBSOCK_CLIENT_UTILS_H
#define WEBSOCK_CLIENT_UTILS_H
#ifdef TODO
#include <websockclient/WebSockClientCommon.h>
#include <websockclient/WebSockClientUtils.h>

BEGIN_EXTERN_C

unsigned int swapEndianess32(unsigned int number);

unsigned long long swapEndianess64(unsigned long long number)
{
	return ((unsigned long long)swapEndianess32(number & 0xFFFFFFFF) << 32) |
		((unsigned long long)swapEndianess32(number >> 32));
}


unsigned int swapEndianess32(unsigned int number)
{
	return (number & 0xFF000000) >> 24 |
		(number & 0x00FF0000) >> 8 |
		(number & 0x000000FF) << 24 |
		(number & 0x0000FF00) << 8;
}

unsigned short swapEndianess16(unsigned short number)
{
	return (number & 0xFF00) >> 8 |
		(number & 0x00FF) << 8;
}

unsigned char swapEndianess8(unsigned char number)
{
	return (number & 0xFF00) >> 4 | (number & 0x00FF) << 4;
}

END_EXTERN_C

#endif
#endif