#include "importer.h"
#include <assert.h>
#include "mitab/mitab.h"

#define DBTYPE_2_STRING(type)				\
	(type == SQLITE_TEXT ? "Text" :         \
	type == SQLITE_INTEGER ? "Integer" :	\
	type == SQLITE_FLOAT ? "Real" :			\
	"Blob")

int mapinfo_type_to_db(TABFieldType type)
{
	switch(type)
	{
	case TABFChar:
	case TABFDecimal:
		return SQLITE_TEXT;
	case TABFInteger:
	case TABFSmallInt:
		return SQLITE_INTEGER;
	case TABFFloat:
		return SQLITE_FLOAT;
	default:
		return SQLITE_TEXT;
	}
}

const char* file_ext(const char* path)
{
	while(*path != 0 && *path != '.')path++;
	return path;
}

int import_mapinfo_tab(sqlite3* db,const char* tab_path,const char* table,const char* ext)
{
	assert(db != 0);
	assert(tab_path != 0);
	assert(table != 0);

	IMapInfoFile* mapinfo = 0;
	if(sqlite3_stricmp(ext,".tab") == 0)
	{
		mapinfo = new TABFile();
	}
	else if(sqlite3_stricmp(ext,".mif") == 0)
	{
		mapinfo = new MIFFile();
	}

	if (mapinfo == 0)return 1;
	int result = mapinfo->Open(tab_path,"r");
	if (result == -1)return 1;

	OGRFeatureDefn* feaDefn = mapinfo->GetLayerDefn();

	std::string stmt_sql = "INSERT INTO ";
	std::string sql = "DROP TABLE IF EXISTS ";
	sql += table;

	result = sqlite3_exec(db,sql.c_str(),0,0,0);
	if(result != SQLITE_OK)return 1;

	sql = "CREATE TABLE ";
	sql = sql + table + "(OGC_FID INTEGER PRIMARY KEY,Geometry BLOB,";

	stmt_sql = stmt_sql + table + " VALUES(?,?,";

	for(int i = 0;i<feaDefn->GetFieldCount();i++)
	{
		OGRFieldDefn* feildDefn = feaDefn->GetFieldDefn(i);
		sql =  sql + "'" +feildDefn->GetNameRef() + "' "; 
		int dbtype = mapinfo_type_to_db(mapinfo->GetNativeFieldType(i));
		sql = sql + DBTYPE_2_STRING(dbtype);

		stmt_sql += "?";

		sql += (i == (feaDefn->GetFieldCount()-1)) ? ")" : ",";
		stmt_sql += (i == (feaDefn->GetFieldCount()-1)) ? ")" : ",";
	}

	sqlite3_exec(db,"begin",0,0,0);
	result = sqlite3_exec(db,sql.c_str(),0,0,0);
	if(result != SQLITE_OK)
	{
		sqlite3_exec(db,"rollback",0,0,0);
		return 1;
	}

	sqlite3_stmt* stmt = 0;
	result = sqlite3_prepare_v2(db,stmt_sql.c_str(),-1,&stmt,0);
	if(result != SQLITE_OK)
	{
		sqlite3_exec(db,"rollback",0,0,0);
		return 1;
	}

	int nFeatureId = -1;

	while ((nFeatureId = mapinfo->GetNextFeatureId(nFeatureId)) != -1) 
	{
		int bindId = 1;
		TABFeature* feature = mapinfo->GetFeatureRef(nFeatureId);
		if (feature != 0) {
			sqlite3_bind_int(stmt,bindId++,nFeatureId);

			// bind geometry
			OGRGeometry* geometry = feature->GetGeometryRef();
			int wkbSize = geometry->WkbSize();
			unsigned char* buffer = new unsigned char[wkbSize];
			geometry->exportToWkb(wkbXDR,buffer);

			sqlite3_bind_blob(stmt,bindId++,buffer,wkbSize,0);

			// bind fields
			for(int i = 0;i<feaDefn->GetFieldCount();i++)
			{
				const char* value = feature->GetFieldAsString(i);
				sqlite3_bind_text(stmt,bindId++,value,-1,SQLITE_TRANSIENT);
			}

			sqlite3_step(stmt);
			sqlite3_reset(stmt);
		}
	}

	sqlite3_finalize(stmt);
	sqlite3_exec(db,"commit",0,0,0);
	mapinfo->Close();
	delete mapinfo;
	mapinfo = NULL;
	return 0;
}