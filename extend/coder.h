#ifndef CODER_H
#define CODER_H

#include "geos/capi/geos_c.h"

#ifdef __cplusplus
extern "C" {
#endif

// user must free memory
char* polyline_encode(GEOSGeometry* geometry,int point);

// user must free geometry
GEOSGeometry* polyline_decode(const unsigned char* strcode,int size);

#ifdef __cplusplus
}
#endif

#endif