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

void _msg_handler(const char* fmt,...)
{
	// nothing
}

void _init_geos()
{
	initGEOS(_msg_handler,_msg_handler);
}

GEOSGeometry *_geo_from_wkt(const unsigned char *wkt)
{
	return GEOSGeomFromWKT((const unsigned char*)wkt);
}

GEOSGeometry *_geo_from_wkb(const unsigned char *wkb,size_t size)
{
	return GEOSGeomFromWKB_buf(wkb,size);
}

void utf8(sqlite3_context *context,int argc,sqlite3_value **argv)
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

void gbk(sqlite3_context *context,int argc,sqlite3_value **argv)
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

void geo_wkt(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		char* wkt;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			wkt = GEOSGeomToWKT(geometry);
			sqlite3_result_text(context,wkt,-1,0);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_wkb(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_TEXT)
	{ 
		GEOSGeometry* geometry;
		const char* wkt = (const char*)sqlite3_value_text(argv[0]);
		unsigned char* wkb;
		size_t size;

		_init_geos();
		geometry = _geo_from_wkt(wkt);
		if(geometry != 0)
		{
			wkb = GEOSGeomToWKB_buf(geometry,&size);
			sqlite3_result_blob(context,(const void*)wkb,size,0);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_type(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		char* type;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			type = GEOSGeomType(geometry);
			sqlite3_result_text(context,type,-1,0);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_minx(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		Rect rect;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			_get_envelope(geometry,&rect);
			sqlite3_result_double(context,rect.minX);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_miny(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		Rect rect;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			_get_envelope(geometry,&rect);
			sqlite3_result_double(context,rect.minY);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_maxx(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		Rect rect;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			_get_envelope(geometry,&rect);
			sqlite3_result_double(context,rect.maxX);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_maxy(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		Rect rect;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			_get_envelope(geometry,&rect);
			sqlite3_result_double(context,rect.maxY);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_points(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			int num_points = GEOSGetNumCoordinates(geometry);
			sqlite3_result_int(context,num_points);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_area(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
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


void geo_length(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
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

void geo_empty(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			char empty = (GEOSisEmpty(geometry) == 1) ? 1 : 0;
			sqlite3_result_int(context,empty);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_valid(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			char empty = (GEOSisValid(geometry) == 1) ? 1 : 0;
			sqlite3_result_int(context,empty);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_subgeos(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
		if(geometry != 0)
		{
			int sub_geos = GEOSGetNumGeometries(geometry);
			sqlite3_result_int(context,sub_geos);
		}
		GEOSGeom_destroy(geometry);
		finishGEOS();
	}
}

void geo_x(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
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

void geo_y(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
	{ 
		GEOSGeometry* geometry;
		const void* data = sqlite3_value_blob(argv[0]);
		size_t data_size = sqlite3_value_bytes(argv[0]);

		_init_geos();
		geometry = _geo_from_wkb((const unsigned char*)data,data_size);
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

void geo_bound(sqlite3_context *context,int argc,sqlite3_value **argv)
{
	if(argc >= 1)
	{
		const unsigned char* ogc;
		unsigned char* ret_geo_buf;
		size_t size;
		double x1,x2,y1,y2;
		GEOSCoordSequence* seq = 0;
		GEOSGeometry* geometry = 0;
		GEOSGeometry* middle_geo = 0;
		
		_init_geos();
		if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_BLOB)
		{
			size = sqlite3_value_bytes(argv[0]);
			ogc = (const unsigned char*)sqlite3_value_blob(argv[0]);
			middle_geo = _geo_from_wkb(ogc,size);
		}
		else if(argc == 1 && sqlite3_value_type(argv[0]) == SQLITE_TEXT)
		{
			ogc = sqlite3_value_text(argv[0]);
			middle_geo = _geo_from_wkt(ogc);
		}
		else if(argc == 4)
		{
			x1 = sqlite3_value_double(argv[0]);
			y1 = sqlite3_value_double(argv[1]);
			x2 = sqlite3_value_double(argv[2]);
			y2 = sqlite3_value_double(argv[3]);

			seq = GEOSCoordSeq_create(2,2);
			GEOSCoordSeq_setX(seq,0,x1);
			GEOSCoordSeq_setY(seq,0,y1);
			GEOSCoordSeq_setX(seq,1,x2);
			GEOSCoordSeq_setY(seq,1,y2);

			middle_geo = GEOSGeom_createLineString(seq);
		}

		if(middle_geo != 0)
		{
			geometry = GEOSEnvelope(middle_geo);
			if(geometry != 0)
			{
				ret_geo_buf = GEOSGeomToWKB_buf(geometry,&size);
				sqlite3_result_blob(context,ret_geo_buf,size,0);
				GEOSGeom_destroy(geometry);
			}
			GEOSGeom_destroy(middle_geo);
		}

		finishGEOS();
	}
}

void addextendfunctions(sqlite3* db)
{
	ADD_EXTEND_FUNTION(utf8,1);
	ADD_EXTEND_FUNTION(gbk,1);
	ADD_EXTEND_FUNTION(geo_wkb,1);
	ADD_EXTEND_FUNTION(geo_wkt,1);
	ADD_EXTEND_FUNTION(geo_type,1);
	ADD_EXTEND_FUNTION(geo_minx,1);
	ADD_EXTEND_FUNTION(geo_maxx,1);
	ADD_EXTEND_FUNTION(geo_miny,1);
	ADD_EXTEND_FUNTION(geo_maxy,1);
	ADD_EXTEND_FUNTION(geo_points,1);
	ADD_EXTEND_FUNTION(geo_area,1);
	ADD_EXTEND_FUNTION(geo_length,1);
	ADD_EXTEND_FUNTION(geo_empty,1);
	ADD_EXTEND_FUNTION(geo_valid,1);
	ADD_EXTEND_FUNTION(geo_subgeos,1);
	ADD_EXTEND_FUNTION(geo_x,1);
	ADD_EXTEND_FUNTION(geo_y,1);
	ADD_EXTEND_FUNTION(geo_bound,-1);
}

