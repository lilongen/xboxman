// MapExImporter.h: interface for the CMapExImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPEXIMPORTER_H__77BF7478_CD02_4740_AE0D_5BB1920BD628__INCLUDED_)
#define AFX_MAPEXIMPORTER_H__77BF7478_CD02_4740_AE0D_5BB1920BD628__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Map.h"
#include "FileM.h"
#define MAP2MAP_ENTRYS 5

class CMapExImporter  
{
public:
	struct map_to_map {
		byte inner;
		byte outer;
	};
	CMapExImporter();
	virtual ~CMapExImporter();
	
public:
	int  do_import();
	bool exportMap();
	bool importMap(int rows, int cols, char* data);
	byte get_inner_from_outer(byte outer);
	byte get_outer_from_inner(byte inner);
	void define_map2map(	byte floor, 
							byte wall,
							byte destination,
							byte box,
							byte man);
	

private:
	CMap m_map;
	map_to_map m_map2map[MAP2MAP_ENTRYS];
	char m_data[512];
	
};

#endif // !defined(AFX_MAPEXIMPORTER_H__77BF7478_CD02_4740_AE0D_5BB1920BD628__INCLUDED_)
