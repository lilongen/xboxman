// History.h: interface for the CHistory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTORY_H__30EA2960_53A0_4F71_A2EB_FFE3CEE8E838__INCLUDED_)
#define AFX_HISTORY_H__30EA2960_53A0_4F71_A2EB_FFE3CEE8E838__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define HISTORY_PAGE_SIZE 256

typedef struct {
	POINT	grid_man_on;
	INT		step;	
	INT		affected_amount;
} HISTORY_ENTRY;

class CHistory  
{
public:
	CHistory();
	virtual ~CHistory();

	void init(BOOL init_history = FALSE);
	void add_page();
	void del_pages(INT from_page);
	void add(POINT* grid, INT step, INT	affected_amount);
	BOOL get(OUT HISTORY_ENTRY* entry, INT offset = -1 /*available: 1, -1   default: -1*/);
	void reset_length();
	INT	 get_length();
	INT	 get_initial_cursor();
	BOOL is_play_over();
	INT  get_cursor();
	void set_cursor(INT cursor);
	void set_cursor_to_start();
	BOOL load_from_file(LPTSTR filename, OUT FILETIME* pass_last_modified);
	BOOL save_to_file(LPTSTR filename, IN FILETIME* pass_last_modified);
	
	
private:
	void allocate_buffer();

private:
	HISTORY_ENTRY** m_pp_data;
	INT m_page_amount;
	INT m_length;
	INT m_cursor;
	INT m_cursor_initial;
};

#endif // !defined(AFX_HISTORY_H__30EA2960_53A0_4F71_A2EB_FFE3CEE8E838__INCLUDED_)
