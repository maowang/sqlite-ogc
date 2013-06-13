#ifndef EXT_FUNCTION_H
#define EXT_FUNCTION_H

#include <assert.h>
#include <string.h>
#include "sqlite3/sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

// convert gbk to utf8
void ext_utf8(sqlite3_context *context,int argc,sqlite3_value **argv);
void ext_gbk(sqlite3_context *context,int argc,sqlite3_value **argv);

// convert wkb to wkt
void ext_geo_wkt(sqlite3_context *context,int argc,sqlite3_value **argv);
void ext_geo_type(sqlite3_context *context,int argc,sqlite3_value **argv);
void ext_geo_minx(sqlite3_context *context,int argc,sqlite3_value **argv);
void ext_geo_miny(sqlite3_context *context,int argc,sqlite3_value **argv);
void ext_geo_maxx(sqlite3_context *context,int argc,sqlite3_value **argv);
void ext_geo_maxy(sqlite3_context *context,int argc,sqlite3_value **argv);

// add extend function to db
void addextendfunctions(sqlite3* db);

#ifdef __cplusplus
}
#endif

#endif

