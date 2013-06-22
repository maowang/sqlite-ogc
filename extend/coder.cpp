#include <vector>
#include <math.h>
#include <assert.h>

#include "coder.h"

#define DECODE_ZIG_ZAG(n) (((n) >> 1) ^ -((n) & 1))
#define ENCODE_ZIG_ZAG(n) (((n) << 1) ^ ((n) >> 31))

static void _encode_double(std::string& out,double value,int point)
{
	int num = (int)(value*point);		
	char enchar;

	num = ENCODE_ZIG_ZAG(num);

	while(num >= 0x20)
	{
		enchar = ((num & 0x1F) | 0x20) + 63;	  
		out.push_back(enchar);
		num = num >> 5;	   
	}  

	out.push_back(num + 63);
}

static double _decode_double(const std::string& in,unsigned int& offset,int point)
{
	assert(offset < in.size());

	unsigned char ch = 0;
	int ival = 0;
	int shift = 0;

	do
	{
		ch = in[offset++] - 63;
		ival = ival | ((ch & 0x1F) << shift);
		shift += 5;
	} while (ch >= 0x20);

	ival = DECODE_ZIG_ZAG(ival);
	return (double)ival / point;
}

// user must free memory
char* polyline_encode(GEOSGeometry* geometry,int point)
{
	std::string result;
	char* cstr = 0;
	std::vector<double> dvalues;
	double x,y,lastx,lasty;

	if(geometry != NULL)
	{
		switch(GEOSGeomTypeId(geometry))
		{
		case GEOS_POINT:
		case GEOS_LINESTRING:
		case GEOS_LINEARRING:
			{
				unsigned int pnts = 0;
				const GEOSCoordSequence * seq = GEOSGeom_getCoordSeq(geometry);
				GEOSCoordSeq_getSize(seq,&pnts);
				for(unsigned int i=0;i<pnts;i++)
				{
					GEOSCoordSeq_getX(seq,i,&x);
					GEOSCoordSeq_getY(seq,i,&y);

					if(i == 0)
					{
						dvalues.push_back(x);
						dvalues.push_back(y);
					}
					else
					{
						dvalues.push_back(x-lastx);
						dvalues.push_back(y-lasty);
					}

					lastx = x;
					lasty = y;
				}
				break;
			}
		}

		if(dvalues.size() > 0)
		{
			for(unsigned int i=0;i<dvalues.size();i++)
			{
				_encode_double(result,dvalues[i],point);
			}
		}
	}

	if(result.size() > 0)
	{
		int size = result.size()+1;
		cstr = (char*)malloc(size);
		if(cstr != NULL)
		{
			memset(cstr,0,size);
			memcpy(cstr,result.c_str(),result.size());
		}
	}

	return cstr;
}

// user must free geometry
GEOSGeometry* polyline_decode(const unsigned char* strcode,int point)
{
	if(strcode == NULL)return NULL;

	double x,y,lastx,lasty;
	GEOSGeometry* geo = NULL;
	GEOSCoordSequence* seq = NULL;
	std::vector<double> points;
	std::string strBuf = (char*)strcode;
	unsigned int offset = 0;

	lastx = x = _decode_double(strBuf,offset,point);
	lasty = y = _decode_double(strBuf,offset,point);
	points.push_back(x);
	points.push_back(y);

	while(offset < strBuf.size())
	{
		x = lastx + _decode_double(strBuf,offset,point);
		y = lasty + _decode_double(strBuf,offset,point);
		points.push_back(x);
		points.push_back(y);
		lastx = x;
		lasty = y;
	}

	seq = GEOSCoordSeq_create(points.size()/2,2);
	if(seq != NULL)
	{
		for(unsigned int i=0;i<points.size()/2;i++)
		{
			GEOSCoordSeq_setX(seq,i,points[i*2]);
			GEOSCoordSeq_setY(seq,i,points[i*2 + 1]);
		}

		if(points.size() == 2)
		{
			return GEOSGeom_createPoint(seq);
		}
		else
		{
			return GEOSGeom_createLineString(seq);
		}
	}

	return NULL;
}