// MapExImporter.cpp: implementation of the CMapExImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapExImporter.h"

void get_full_path(LPTSTR sz_path, INT length, LPTSTR file_name = 0);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapExImporter::CMapExImporter()
{
	m_map2map[0].inner = (MAP_ELEMENT) FLOOR;
	m_map2map[1].inner = (MAP_ELEMENT) WALL;
	m_map2map[2].inner = (MAP_ELEMENT) DESTINATION;
	m_map2map[3].inner = (MAP_ELEMENT) BOX * 16 + FLOOR;
	m_map2map[4].inner = (MAP_ELEMENT) MAN * 16 + FLOOR;

	ZeroMemory(m_data, 512);
}

CMapExImporter::~CMapExImporter()
{

}

bool CMapExImporter::exportMap()
{
	return true;
}

bool CMapExImporter::importMap(int rows, int cols, char* data)
{
	m_map.init();
	m_map.m_p_ph->rows = rows;
	m_map.m_p_ph->cols = cols;
	
	for (int i = 0; i < rows; i ++)
	{
		for (int j = 0; j < cols; j ++)
		{
			char ch = data[i * cols + j];
			byte grid = atoi(&ch);
			m_map.m_p_map[i * cols + j] = get_inner_from_outer(grid);
		}
	}
	
	_g_pass_no = -1;
	m_map.save();

	return true;
}

void CMapExImporter::define_map2map(	byte floor, 
											byte wall,
											byte destination,
											byte box,
											byte man)
{
	m_map2map[0].outer = floor;
	m_map2map[1].outer = wall;
	m_map2map[2].outer = destination;
	m_map2map[3].outer = box;
	m_map2map[4].outer = man;
}

byte CMapExImporter::get_inner_from_outer(byte outer)
{
	for (int i = 0; i < MAP2MAP_ENTRYS; i ++)
	{
		if (m_map2map[i].outer == outer)
			return m_map2map[i].inner;
	}

	return 0;
}

byte CMapExImporter::get_outer_from_inner(byte inner)
{
	for (int i = 0; i < MAP2MAP_ENTRYS; i ++)
	{
		if (m_map2map[i].inner == inner)
			return m_map2map[i].outer;
	}

	return 0;
}

int CMapExImporter::do_import()
{
	TCHAR full_path[256] = {0};
	get_full_path(full_path, 256, "pass_lib");
	char sz_sect[32]	= {0};
	char sz_key[32]		= {0};
	char sz_key_value[128] = {0};
	int rows = 0;
	int cols = 0;
	/*
	define_map2map(byte floor, 
						byte wall,
						byte destination,
						byte box,
						byte man);
	*/
	define_map2map(1, 
						2, 
						4, 
						3, 
						5);
	byte a = get_inner_from_outer(3);
	for (int i = 0; i < 13; i++)
	{
		ZeroMemory(m_data, 512);
		ZeroMemory(sz_sect, 32);
		wsprintf(sz_sect, "Map%d", i + 1);
		
		ZeroMemory(sz_key_value, 128);
		::GetPrivateProfileString(sz_sect, "MapWidth", NULL, sz_key_value, 128, full_path);
		cols = atoi(sz_key_value);

		ZeroMemory(sz_key_value, 128);
		::GetPrivateProfileString(sz_sect, "MapHeight", NULL, sz_key_value, 128, full_path);
		rows = atoi(sz_key_value);
		
		for (int j = 0; j < rows; j ++)
		{
			ZeroMemory(sz_key, 32);
			wsprintf(sz_key, "MapLine%d", j + 1);

			::GetPrivateProfileString(sz_sect, sz_key, NULL, sz_key_value, 128, full_path);
			memcpy(m_data + (j * cols), sz_key_value, cols);
		}

		importMap(rows, cols, m_data);
	}
	
	return 1;
}