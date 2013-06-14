#include "extend/extfunction.h"
#include "iconv/include/iconv.h"
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
	unsigned int loop=0;
	assert(rect != 0);
	assert(geometry != 0);
	rect->minX = rect->minY = rect->maxX = rect->maxY = 0;

	if(GEOSGeomTypeId(geometry) == GEOS_POINT)
	{
		GEOSGeomGetX(geometry,&x);
		GEOSGeomGetY(geometry,&y);
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

void _ext_msg_handler(const char* fmt,...)
{
	// nothing
}

void _ext_init_geos()
{
	initGEOS(_ext_msg_handler,_ext_msg_handler);
}

GEOSGeometry *_ext_geo_from_wkt(const char *wkt)
{
	return GEOSGeomFromWKT(wkt);
}

GEOSGeometry *_ext_geo_from_wkb(const unsigned char *wkb,size_t size)
{
	return GEOSGeomFromWKB_buf(wkb,size);
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

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
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
		const char* wkt = (const char*)sqlite3_value_text(argv[0]);
		unsigned char* wkb;
		size_t size;

		_ext_init_geos();
		geometry = _ext_geo_from_wkt(wkt);
		if(geometry != 0)
		{
			wkb = GEOSGeomToWKB_buf(geometry,&size);
			sqlite3_result_blob(context,(const void*)wkb,size,0);
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

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
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

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
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

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
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

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
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

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			_get_envelope(geometry,&rect);
			sqlite3_result_double(context,rect.maxY);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_points(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			int num_points = GEOSGetNumCoordinates(geometry);
			sqlite3_result_int(context,num_points);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_area(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			double area = 0;
			GEOSArea(geometry,&area);
			sqlite3_result_double(context,area);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}


void ext_geo_length(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			double length = 0;
			GEOSLength(geometry,&length);
			sqlite3_result_double(context,length);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_empty(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			char empty = (GEOSisEmpty(geometry) == 1) ? 1 : 0;
			sqlite3_result_int(context,empty);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_valid(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			char empty = (GEOSisValid(geometry) == 1) ? 1 : 0;
			sqlite3_result_int(context,empty);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_subgeos(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			int sub_geos = GEOSGetNumGeometries(geometry);
			sqlite3_result_int(context,sub_geos);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_x(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			if(GEOSGeomTypeId(geometry) == GEOS_POINT)
			{
				double x;
				GEOSGeomGetX(geometry,&x);
				sqlite3_result_double(context,x);
			}
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void ext_geo_y(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_ext_init_geos();
		geometry = _ext_geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			if(GEOSGeomTypeId(geometry) == GEOS_POINT)
			{
				double y;
				GEOSGeomGetY(geometry,&y);
				sqlite3_result_double(context,y);
			}
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

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
	ADD_EXTEND_FUNTION(ext_geo_points,1);
	ADD_EXTEND_FUNTION(ext_geo_area,1);
	ADD_EXTEND_FUNTION(ext_geo_length,1);
	ADD_EXTEND_FUNTION(ext_geo_empty,1);
	ADD_EXTEND_FUNTION(ext_geo_valid,1);
	ADD_EXTEND_FUNTION(ext_geo_subgeos,1);
	ADD_EXTEND_FUNTION(ext_geo_x,1);
	ADD_EXTEND_FUNTION(ext_geo_y,1);
}

