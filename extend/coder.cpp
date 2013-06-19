#include "coder.h"

#define DECODE_ZIG_ZAG(n) (((n) >> 1) ^ -((n) & 1))
#define ENCODE_ZIG_ZAG(n) (((n) << 1) ^ ((n) >> 31)

// user must free memory
char* polyline_encode(GEOSGeometry* geometry,int* size)
{
	return 0;
}

// user must free geometry
GEOSGeometry* polyline_decode(const char* strcode,int size)
{
	return 0;
}