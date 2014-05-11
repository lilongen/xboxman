// History.cpp: implementation of the CHistory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "History.h"

#include "FileM.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHistory::CHistory()
{
	m_cursor_initial	= -1;
	m_cursor			= -1;
	m_length			= 0;
	m_page_amount		= 0;

	m_pp_data = (HISTORY_ENTRY**) new LONG[256];
	init(FALSE);
}

CHistory::~CHistory()
{
	del_pages(0);
	delete[] m_pp_data;
}

void CHistory::init(BOOL init_history)
{
	del_pages(0);
	ZeroMemory(m_pp_data, 256 * sizeof(LONG));  // put ** to zero and keep only allocated page has an HISTORY_ENTRY* value
	
	m_cursor_initial	= -1;
	m_cursor			= -1;
	m_length			= 0;
	m_page_amount		= 0;
}

void CHistory::allocate_buffer()  //used for load history file and construct history data
{
	for (INT i = 0; i < m_page_amount; i++)
	{
		*(m_pp_data + i) = new HISTORY_ENTRY[HISTORY_PAGE_SIZE];
		ZeroMemory(*(m_pp_data + i), HISTORY_PAGE_SIZE * sizeof(HISTORY_ENTRY));
	}
}

void CHistory::add_page()
{
	*(m_pp_data + m_page_amount) = new HISTORY_ENTRY[HISTORY_PAGE_SIZE];
	ZeroMemory(*(m_pp_data + m_page_amount), HISTORY_PAGE_SIZE * sizeof(HISTORY_ENTRY));
	
	m_page_amount ++;
}

void CHistory::del_pages(INT from_page)
{
	for (INT i = from_page; i < m_page_amount; i ++)
	{
		if (*(m_pp_data + i) != NULL)
		{
			delete[] *(m_pp_data + i);
			*(m_pp_data + i) = NULL;
		}

	}
}

void CHistory::reset_length()
{
	if (m_length != 0 && m_cursor != m_length - 1)
		m_length = m_cursor + 1;
}

void CHistory::add(POINT* grid, INT step, INT affected_amount)
{
	if (m_length >= m_page_amount * HISTORY_PAGE_SIZE)
		add_page();
	
	int page_no = m_length / HISTORY_PAGE_SIZE;
	INT offset_on_page = m_length % HISTORY_PAGE_SIZE;
	
	HISTORY_ENTRY* history_entry = *(m_pp_data + page_no) + offset_on_page;
	memcpy(&history_entry->grid_man_on, grid, sizeof(POINT));
	history_entry->step = step;
	history_entry->affected_amount = affected_amount;

	m_cursor = m_length;
	m_cursor_initial = m_cursor;
	m_length ++;

#ifdef __DEBUG__
TCHAR szDebug[128] = {0};
wsprintf(szDebug, "m_page_amount: %d, m_length: %d, m_cursor: %d ", m_page_amount, m_length, m_cursor);
wsprintf(szDebug, "%s   man_on(%d, %d), step: %d, attected_amount %d\r\n", szDebug, grid->y, grid->x, step, affected_amount);
CFileM file(DEBUG_FILE, OPEN_ALWAYS);
file.set_file_pointer(0, FILE_END);
file.write(szDebug, lstrlen(szDebug));
#endif
}

BOOL CHistory::get(OUT HISTORY_ENTRY* entry, INT offset /*available: 1, -1   default: -1*/)
{
	if (offset > 0)
	{
		m_cursor += offset;
		if (m_cursor > m_cursor_initial)
		{
			m_cursor -= offset;
			return FALSE;
		}
	}

	if (m_cursor < 0 || m_cursor >= m_length)
		return FALSE;

	int page_no = m_cursor / HISTORY_PAGE_SIZE;
	INT offset_on_page = m_cursor % HISTORY_PAGE_SIZE;
	
	HISTORY_ENTRY* history_entry = *(m_pp_data + page_no) + offset_on_page;
	memcpy(entry, history_entry, sizeof(HISTORY_ENTRY));
	if (offset < 0)
		m_cursor += offset;

	return TRUE;
}

INT	CHistory::get_length()
{
	return m_length;
}

INT	CHistory::get_initial_cursor()
{

	return m_cursor_initial;
}

INT CHistory::get_cursor()
{
	return m_cursor;
}

void CHistory::set_cursor(INT cursor)
{
	m_cursor = cursor;
}

BOOL CHistory::load_from_file(LPTSTR filename, OUT FILETIME* pass_last_modified)
{
	CFileM file(filename, OPEN_EXISTING);
	if (INVALID_HANDLE_VALUE == file.m_h_file)
		return FALSE;
	
// file format
//	| _g_pass_no		|
//	| m_page_amount		|
//	| m_length			|
//	| m_cursor			|
//	| m_cursor_initial	|
//  | history data		|
	init(TRUE);
	file.set_file_pointer(0);
	file.read(&_g_pass_no,			sizeof(INT));
	file.read(pass_last_modified,	sizeof(FILETIME));
	file.read(&m_page_amount,		sizeof(INT));
	file.read(&m_length,			sizeof(INT));
	file.read(&m_cursor,			sizeof(INT));
	file.read(&m_cursor_initial,	sizeof(INT));
	m_cursor_initial	= m_length - 1;
	m_cursor			= -1;
	allocate_buffer();

	for (INT i = 0; i < m_page_amount; i ++)
	{
		file.read(*(m_pp_data + i), HISTORY_PAGE_SIZE * sizeof(HISTORY_ENTRY));
	}
	return TRUE;
}

void CHistory::set_cursor_to_start()
{
	m_cursor = -1;
	m_cursor_initial = m_length - 1;
}

BOOL CHistory::save_to_file(LPTSTR filename, IN FILETIME* pass_last_modified)
{
	CFileM file(filename, CREATE_ALWAYS);
	if (INVALID_HANDLE_VALUE == file.m_h_file)
		return FALSE;
	
	file.set_file_pointer(0);
// file format
//	| _g_pass_no		|
//	| m_page_amount		|
//	| m_length			|
//	| m_cursor			|
//	| m_cursor_initial	|
//  | history data		|

	file.write(&_g_pass_no,			sizeof(INT));
	file.write(pass_last_modified,	sizeof(FILETIME));
	file.write(&m_page_amount,		sizeof(INT));
	file.write(&m_length,			sizeof(INT));
	file.write(&m_cursor,			sizeof(INT));
	file.write(&m_cursor_initial,	sizeof(INT));
	for (INT i = 0; i < m_page_amount; i ++)
	{
		file.write(*(m_pp_data + i), HISTORY_PAGE_SIZE * sizeof(HISTORY_ENTRY));
	}

	return TRUE;
}

BOOL CHistory::is_play_over()
{
	return (m_cursor == m_cursor_initial);
}