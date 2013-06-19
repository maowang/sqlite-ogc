#include <vector>
#include <math.h>

#include "coder.h"

#define DECODE_ZIG_ZAG(n) (((n) >> 1) ^ -((n) & 1)))
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
GEOSGeometry* polyline_decode(const char* strcode,int size)
{
	return 0;
}