#ifndef FUNCTION_H
#define FUNCTION_H

#include <assert.h>
#include <string.h>
#include "sqlite3/include/sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADD_EXTEND_FUNTION(x,n) sqlite3_create_function_v2(db,#x,n,SQLITE_ANY,0,x,0,0,0);

// convert gbk to utf8
void utf8(sqlite3_context *context,int argc,sqlite3_value **argv);
void gbk(sqlite3_context *context,int argc,sqlite3_value **argv);

// convert wkb to wkt
void geo_wkt(sqlite3_context *context,int argc,sqlite3_value **argv);
void geo_type(sqlite3_context *context,int argc,sqlite3_value **argv);
void geo_minx(sqlite3_context *context,int argc,sqlite3_value **argv);
void geo_miny(sqlite3_context *context,int argc,sqlite3_value **argv);
void geo_maxx(sqlite3_context *context,int argc,sqlite3_value **argv);
void geo_maxy(sqlite3_context *context,int argc,sqlite3_value **argv);

void geo_points(sqlite3_context *context,int argc,sqlite3_value **argv);
void geo_area(sqlite3_context *context,int argc,sqlite3_value **argv);
void geo_length(sqlite3_context *context,int argc,sqlite3_value **argv);

void geo_empty(sqlite3_context *context,int argc,sqlite3_value **argv);
void geo_valid(sqlite3_context *context,int argc,sqlite3_value **argv);
void geo_subgeos(sqlite3_context *context,int argc,sqlite3_value **argv);

void geo_x(sqlite3_context *context,int argc,sqlite3_value **argv);
void geo_y(sqlite3_context *context,int argc,sqlite3_value **argv);

void geo_bound(sqlite3_context *context,int argc,sqlite3_value **argv);

// add extend function to db
void addextendfunctions(sqlite3* db);

#ifdef __cplusplus
}
#endif

#endif

