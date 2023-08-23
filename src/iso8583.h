#pragma once
#include <stdio.h>
typedef unsigned char PrimitiveByteValue;

struct ByteValue {
	PrimitiveByteValue* raw;
	int length;
};


enum ConverterIdx {
	BitConverter,
	AsciiConverter,
	NumericConverter,
	BCDConverter
};

typedef int CVX_ID;
#if 0 

struct ConvertFn {
	CVX_ID id;
	char* (*byte_to_string)(ByteValue*,int length, int length_type);
	ByteValue* (*string_to_byte)(char*, int length, int length_type);
};

char *convert_byte_to_string(ByteValue * data,int length, int length_type){}
ByteValue* convert_string_to_byte(char* data, int length, int length_type) {}

char* convert_ascii_to_string(ByteValue* data, int length, int length_type) {}
ByteValue* convert_string_to_ascii(char* data, int length, int length_type) {}

char* convert_numeric_to_string(ByteValue* data, int length, int length_type) {}
ByteValue* convert_string_to_numeric(char* data, int length, int length_type) {}

char* convert_bcd_to_string(ByteValue* data, int length, int length_type) {}
ByteValue* convert_string_to_bcd(char* data, int length, int length_type) {}


static const ConvertFn DefaultConverters[] =
{
	{ BitConverter, convert_byte_to_string,convert_string_to_byte},
	{ AsciiConverter, convert_ascii_to_string,convert_string_to_ascii},
	{ NumericConverter, convert_numeric_to_string,convert_string_to_numeric},
	{ BCDConverter, convert_bcd_to_string,convert_string_to_bcd},
	{ -1 , NULL, NULL}
};


void testIso8583();

#endif