#include "extend/extfunction.h"
#include "libiconv/include/iconv.h"
#include "geos/capi/geos_c.h"

typedef struct {
	double minX;
	double minY;
	double maxX;
	double maxY;
}Rect;

static void _get_envelope(GEOSGeometry* geometry,Rect* rect)
{
	unsigned int num_pnt;
	GEOSGeometry* env;
	const GEOSGeometry* ring;
	const GEOSCoordSequence* seq;
	double x,y;
	int loop=0;
	assert(rect != 0);
	assert(geometry != 0);
	rect->minX = rect->minY = rect->maxX = rect->maxY = 0;

	if(GEOSGeomTypeId(geometry) == GEOS_POINT)
	{
		GEOSGeomGetX(geometry,&x);
		GEOSGeomGetX(geometry,&y);
		rect->minX = rect->maxX = x;
		rect->minY = rect->maxY = y;
		return;
	}

	env = GEOSEnvelope(geometry);
	ring = GEOSGetExteriorRing(env);
	seq	 =	GEOSGeom_getCoordSeq(ring);
	GEOSCoordSeq_getSize(seq,&num_pnt);

	if(num_pnt == 0 || num_pnt > 5)return;

	GEOSCoordSeq_getX(seq,0,&x);
	GEOSCoordSeq_getY(seq,0,&y);

	rect->minX = rect->maxX = x;
	rect->minY = rect->maxY = y;

	for(loop=1;loop<num_pnt;loop++)
	{
		GEOSCoordSeq_getX(seq,loop,&x);
		GEOSCoordSeq_getY(seq,loop,&y);
		
		if(x > rect->maxX) rect->maxX = x;
		if(x < rect->minX) rect->minX = x;

		if(y > rect->maxY) rect->maxY = y;
		if(y < rect->minY) rect->minY = y;
	}

	GEOSGeom_destroy(env);
}

void ext_utf8(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_TEXT)
	{ 
		const unsigned char* text = sqlite3_value_text(argv[0]);
		size_t textLen = sqlite3_value_bytes(argv[0]);

		size_t outLen = textLen * 4;
		char* out = (char*)sqlite3_malloc(outLen);
		char* pout = out;

		iconv_t handle = iconv_open("utf-8","gbk");

		memset(out,0,outLen);
		iconv(handle,(const char**)&text,&textLen,&pout,&outLen);
		iconv_close(handle);

		sqlite3_result_text(context,out,-1,0);
	}
}

void ext_gbk(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_TEXT)
	{ 
		const unsigned char* text = sqlite3_value_text(argv[0]);
		size_t textLen = sqlite3_value_bytes(argv[0]);

		size_t outLen = textLen * 4;
		char* out = (char*)sqlite3_malloc(outLen);
		char* pout = out;

		iconv_t handle = iconv_open("gbk","utf-8");
		memset(out,0,outLen);
		iconv(handle,(const char**)&text,&textLen,&pout,&outLen);
		iconv_close(handle);

		sqlite3_result_text(context,out,-1,0);
	}
}

void ext_geo_wkt(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		char* wkt;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		initGEOS(0,0);
		geometry = GEOSGeomFromWKB_buf((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			wkt = GEOSGeomToWKT(geometry);
			sqlite3_result_text(context,wkt,-1,0);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_wkb(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_TEXT)
	{ 
		GEOSGeometry* geometry;
		const unsigned char* wkt = sqlite3_value_text(argv[0]);
		unsigned char* wkb;
		size_t size;

		initGEOS(0,0);
		geometry = GEOSGeomFromWKT(wkt);
		if(geometry != 0)
		{
			wkb = GEOSGeomToWKB_buf(geometry,&size);
			sqlite3_result_blob(context,(const void*)wkb,size,0);
			GEOSFree(wkb);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_type(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		char* type;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		initGEOS(0,0);
		geometry = GEOSGeomFromWKB_buf((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			type = GEOSGeomType(geometry);
			sqlite3_result_text(context,type,-1,0);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_minx(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		Rect rect;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		initGEOS(0,0);
		geometry = GEOSGeomFromWKB_buf((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			_get_envelope(geometry,&rect);
			sqlite3_result_double(context,rect.minX);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_miny(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		Rect rect;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		initGEOS(0,0);
		geometry = GEOSGeomFromWKB_buf((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			_get_envelope(geometry,&rect);
			sqlite3_result_double(context,rect.minY);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_maxx(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		Rect rect;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		initGEOS(0,0);
		geometry = GEOSGeomFromWKB_buf((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			_get_envelope(geometry,&rect);
			sqlite3_result_double(context,rect.maxX);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_maxy(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		Rect rect;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		initGEOS(0,0);
		geometry = GEOSGeomFromWKB_buf((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			_get_envelope(geometry,&rect);
			sqlite3_result_double(context,rect.maxY);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

#define ADD_EXTEND_FUNTION(x,n) sqlite3_create_function_v2(db,#x,n,SQLITE_ANY,0,x,0,0,0);

void addextendfunctions(sqlite3* db)
{
	ADD_EXTEND_FUNTION(ext_utf8,1);
	ADD_EXTEND_FUNTION(ext_gbk,1);
	ADD_EXTEND_FUNTION(ext_geo_wkb,1);
	ADD_EXTEND_FUNTION(ext_geo_wkt,1);
	ADD_EXTEND_FUNTION(ext_geo_type,1);
	ADD_EXTEND_FUNTION(ext_geo_minx,1);
	ADD_EXTEND_FUNTION(ext_geo_maxx,1);
	ADD_EXTEND_FUNTION(ext_geo_miny,1);
	ADD_EXTEND_FUNTION(ext_geo_maxy,1);
}

