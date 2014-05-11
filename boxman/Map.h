// Map.h: interface for the CMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAP_H__368F6035_45C1_4299_818A_8D51B2950738__INCLUDED_)
#define AFX_MAP_H__368F6035_45C1_4299_818A_8D51B2950738__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FileM.h"
#define MAP_LENGTH 512

typedef enum {
	EMPTY		= 0,
	FLOOR		= 1,
	WALL		= 2,
	DESTINATION = 3,
	BOX			= 4,
	MAN			= 5,
} MAP_ELEMENT;

typedef struct {
	UINT start;
	UINT end;
	FILETIME last_modified;
} PASS_SEGMENT;

typedef struct {
	UINT pass_amount;
	PASS_SEGMENT pass_segment[256]; 
} PASS_FILE_HEADER;

struct PASS_HEADER {
public:
	void init() {
		style	= 0;
		rows	= 15;
		cols	= 20;
		men		= 1;
	}
	
public:
	INT style;
	INT rows;
	INT cols;
	INT men;
	
};

typedef struct {
	PASS_HEADER pass_header;
	PBYTE pass_map_data;
} PASS_INFO;

class CMap 
{
public:
	CMap();
	virtual ~CMap();

	BOOL	save();
	BOOL	load(UINT pass_no);
	BYTE	get_grid(INT n_row, INT n_col);
	BYTE	get(INT n_offset);
	void	set(INT n_offset, BYTE val);
	void	set_grid(INT n_row, INT n_col, BYTE val);
	POINT	index_to_grid(INT n_index);
	INT		grid_to_index(INT row, INT col);
	void	get_valid_data();
	void	init();
	BOOL	is_passed();
	BOOL	is_map_legal();
	void	get_time_countermark(FILETIME* ptime);
	BOOL	import_map(INT rows, INT cols, CHAR* map_data);
	FILETIME* get_map_last_modified(INT pass_no);
	BOOL	isBarrier(INT index);
	BOOL    isMan(INT index);
	
private:
	void	get_file_header();
	
public:
	PASS_HEADER*		m_p_ph;
	PBYTE				m_p_map;
	PASS_FILE_HEADER*	m_p_file_header;
};

#endif // !defined(AFX_MAP_H__368F6035_45C1_4299_818A_8D51B2950738__INCLUDED_)
