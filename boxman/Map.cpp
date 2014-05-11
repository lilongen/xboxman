// BoxmanMap.cpp: implementation of the CMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Map.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void get_full_path(LPTSTR sz_path, INT length, LPTSTR file_name = 0);
CMap::CMap()
{
	m_p_ph	= new PASS_HEADER;
	m_p_map = new BYTE[MAP_LENGTH];
	m_p_file_header = new PASS_FILE_HEADER;

	init();
}

CMap::~CMap()
{
	if (m_p_ph)
		delete m_p_ph;
	if (m_p_map)
		delete[] m_p_map;
	if (m_p_file_header)
		delete m_p_file_header;
}

void CMap::get_file_header()
{
	ZeroMemory(m_p_file_header, sizeof(PASS_FILE_HEADER));
	TCHAR full_path[256] = {0};
	get_full_path(full_path, 256, PASS_ARCHIVE);
	CFileM file(full_path);
	file.set_file_pointer(0);
	file.read(m_p_file_header, sizeof(PASS_FILE_HEADER));
}

BOOL CMap::save()
{
	INT start, end, length;
	length =  m_p_ph->cols * m_p_ph->rows;
	if (_g_pass_no == -1)
	{
		if (m_p_file_header->pass_amount == 0)
			start = sizeof(PASS_FILE_HEADER);
		else 
		{
			start = m_p_file_header->pass_segment[m_p_file_header->pass_amount - 1].end;
		}

		m_p_file_header->pass_amount ++;
		_g_pass_no = m_p_file_header->pass_amount - 1;
		_g_pass_amount = m_p_file_header->pass_amount;
	}
	else
	{
		if (_g_pass_no == 0)
			start = sizeof(PASS_FILE_HEADER);
		else
			start = m_p_file_header->pass_segment[_g_pass_no - 1].end;
	}

	end = start + length + sizeof(PASS_HEADER);
	
	TCHAR full_path[256] = {0};
	get_full_path(full_path, 256, PASS_ARCHIVE);
	CFileM file(full_path);

	get_time_countermark(&m_p_file_header->pass_segment[_g_pass_no].last_modified);
	file.set_file_pointer(start);
	file.write(m_p_ph, sizeof(PASS_HEADER));
	file.write(m_p_map, length);

	m_p_file_header->pass_segment[_g_pass_no].start = start;
	m_p_file_header->pass_segment[_g_pass_no].end	= end;
	file.set_file_pointer(0);
	file.write(m_p_file_header, sizeof(PASS_FILE_HEADER));
	
	return TRUE;
}

BOOL CMap::load(UINT pass_no)
{
	if (pass_no > m_p_file_header->pass_amount) 
		return FALSE;
	
	TCHAR full_path[256] = {0};
	get_full_path(full_path, 256, PASS_ARCHIVE);
	CFileM file(full_path);
	file.set_file_pointer(m_p_file_header->pass_segment[pass_no].start);
	file.read(m_p_ph, sizeof(PASS_HEADER));
	file.read(m_p_map, m_p_ph->cols * m_p_ph->rows);

	return TRUE;
}

void CMap::init()
{
	m_p_ph->init();
	ZeroMemory(m_p_map, MAP_LENGTH);
	get_file_header();
	_g_pass_amount = m_p_file_header->pass_amount;
}

BYTE CMap::get_grid(INT n_row, INT n_col)
{
	INT offset = n_row * m_p_ph->cols + n_col;
	return *(m_p_map + offset);
}

void CMap::set_grid(INT n_row, INT n_col, BYTE val)
{
	INT offset = n_row * m_p_ph->cols + n_col;
	*(m_p_map + offset) = val;
}

POINT CMap::index_to_grid(INT n_index)
{
	POINT grid;
	grid.y = n_index / m_p_ph->cols;
	grid.x = n_index % m_p_ph->cols;

	return grid;
}
INT CMap::grid_to_index(INT row, INT col)
{
	INT offset = row * m_p_ph->cols + col;

	return offset;
}

BYTE CMap::get(INT n_offset)
{
	return *(m_p_map + n_offset);
}

void CMap::set(INT n_offset, BYTE val)
{
	*(m_p_map + n_offset) = val;
}

void CMap::get_valid_data()
{
	INT rows = m_p_ph->rows;
	INT cols = m_p_ph->cols;

	INT l, t, r, b;
	l = t = r = b = 0;
	INT i, j;
	BOOL l_got, t_got;
	l_got = t_got = FALSE;
	for (j = 0; j < cols; j++)
	{
		for (i = 0; i < rows; i++)		
		{
			if (m_p_map[i * cols + j] > 0)
			{
				if (!l_got)
				{
					l = j;
					l_got = TRUE;
				}
				r = max(r, j);
			}
		}
	}
	
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)				
		{
			if (m_p_map[i * cols + j] > 0)
			{
				if (!t_got)
				{
					t = i;
					t_got = TRUE;
				}
				b = max(b, i);
			}
		}
	}

	int validate_rows = b - t + 1;
	int validate_cols = r - l + 1;
	
	BYTE* data = new BYTE[validate_rows * validate_cols];
	INT offset = 0;
	for (i = t; i < validate_rows + t; i ++)
	{
		for (j = l; j < validate_cols + l; j ++)
		{
			data[offset ++] = m_p_map[i * cols + j];
		}
	}
	m_p_ph->rows = validate_rows;
	m_p_ph->cols = validate_cols;
	ZeroMemory(m_p_map, rows * cols);
	memcpy(m_p_map, data, validate_rows * validate_cols);
	delete[] data;
}

BOOL CMap::is_passed()
{
	for (INT i = 0; i < m_p_ph->cols * m_p_ph->rows; i ++)
	{
		if (	m_p_map[i] % 16 == (MAP_ELEMENT) DESTINATION
			&& (m_p_map[i] / 16 != (MAP_ELEMENT) BOX))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CMap::is_map_legal()
{
	INT destination_amount, box_amount, man_amount;
	destination_amount = box_amount = man_amount = 0;
	BYTE hi4bit, lo4bit;
	for (INT i = 0; i < m_p_ph->cols * m_p_ph->rows; i ++)
	{	
		hi4bit = m_p_map[i] / 16;
		lo4bit = m_p_map[i] % 16;

		if (hi4bit == (MAP_ELEMENT) BOX) 
			box_amount ++;

		if (lo4bit == (MAP_ELEMENT) DESTINATION) 
			destination_amount ++;

		if (hi4bit == (MAP_ELEMENT) MAN) 
			man_amount ++;
	}
	
	BOOL legal = (box_amount == destination_amount) && (man_amount == 1);
	
	return legal;	
}

void CMap::get_time_countermark(FILETIME* ptime)
{
	SYSTEMTIME st;
	::GetSystemTime(&st);
	::SystemTimeToFileTime(&st, ptime);	
}

FILETIME* CMap::get_map_last_modified(INT pass_no)
{
	FILETIME* ft = &(m_p_file_header->pass_segment[pass_no].last_modified);

	return ft;
}

BOOL CMap::isBarrier(INT index)
{
	BYTE hi4bit, lo4bit;
	BYTE val = get(index);
	
	hi4bit = val >> 4;
	lo4bit = val & 0xF;
	if (hi4bit == (MAP_ELEMENT) BOX 
		|| lo4bit == (MAP_ELEMENT) WALL 
		|| lo4bit == (MAP_ELEMENT) EMPTY) {
		
		return true;
	}

	return false;
}

BOOL CMap::isMan(INT index) {
	return (get(index) >> 4 ) == (MAP_ELEMENT) MAN;
}
