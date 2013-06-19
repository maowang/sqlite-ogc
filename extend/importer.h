#ifndef IMPORTER_H
#define IMPORTER_H

#include "sqlite3/include/sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* file_ext(const char* path);

// import mapinfo
// return: successed 0 otherwire 1
int import_mapinfo_tab(sqlite3* db,const char* tab_path,const char* table,const char* ext);

int build_rtree(sqlite3* db,const char* srctable,const char* desttable);
#ifdef __cplusplus
}
#endif

#endif