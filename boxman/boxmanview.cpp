#include "stdafx.h"
#include "BoxmanView.h"

CBoxmanView::CBoxmanView()
{
	m_origin.x = 5;
	m_origin.y = 5;
	m_grid_mouse_on.x = -1;
	m_hdc = NULL;

	m_grid_focus_pen	= ::CreatePen(NULL, 2, RGB(0x00, 0xff, 0x00));
	m_grid_pen			= ::CreatePen(NULL, 1, RGB(0xee, 0xee, 0xee)); 
	m_pen				= ::CreatePen(NULL, 1, RGB(0x00, 0x00, 0x00));
	init_font();
	m_h_info_dlg = NULL;
	load_tips();
}

CBoxmanView::~CBoxmanView()
{
	::DeleteObject(m_grid_focus_pen);
	::DeleteObject(m_grid_pen);
	::DeleteObject(m_pen);
	::DeleteObject(m_hfont_info);
}

BOOL CBoxmanView::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

LRESULT CBoxmanView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(&ps);
	m_hdc = hdc;
	m_mem_hdc = ::CreateCompatibleDC(m_hdc);
	m_mem_hdc_operator = ::CreateCompatibleDC(m_hdc);
	HBITMAP hbmp = ::CreateCompatibleBitmap(m_hdc, m_map_size.cx, m_map_size.cy);
	::SelectObject(m_mem_hdc_operator, hbmp);
	draw_map();
	draw_map_area(m_hdc);
	EndPaint(&ps);
	
	::DeleteObject(hbmp);
	::DeleteDC(m_mem_hdc);
	::DeleteDC(m_mem_hdc_operator);

	return S_OK;
}

LRESULT CBoxmanView::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	if (m_h_info_dlg != NULL && ::IsWindowVisible(m_h_info_dlg)) return S_OK;

	if (_g_game_mode == (GAME_MODE) PLAY_GAME_MODE
		|| _g_game_mode == (GAME_MODE) PLAY_RECORD_MODE )
	{
		return S_OK;
	}

	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	POINT grid = m_ui_assist->mouse_point_to_grid(&pt, &m_origin);
	if (grid.x == -1)
	{
		if (m_grid_mouse_on.x != -1 && m_grid_mouse_on.y != -1)
			remove_focus(m_grid_mouse_on.y, m_grid_mouse_on.x);
		
		m_static_info.SetWindowText(m_info);		
		return S_OK;
	}
	
	HDC hdc = ::GetDC(m_hWnd);
	if (m_grid_mouse_on.x != grid.x || m_grid_mouse_on.y != grid.y)
	{
		if (m_grid_mouse_on.x != -1)
			remove_focus(m_grid_mouse_on.y, m_grid_mouse_on.x);
		
		if (wParam == MK_LBUTTON)
			draw_element(grid.y, grid.x);
		if (wParam == MK_RBUTTON)
			erase_element(grid.y, grid.x);
		
		draw_focus(hdc, grid.y, grid.x);

		if (wParam == MK_LBUTTON || wParam == MK_RBUTTON)
			::SendMessage(m_h_mainframe, WM_DISABLE_MAP_SAVE, NULL, NULL);
	}
	
	int len = lstrlen(m_info);
	wsprintf(m_info + len, "\r\nrow, col  (%d, %d)", grid.y, grid.x);
	m_static_info.SetWindowText(m_info);
	*((CHAR*) m_info + len) = '\0';
	::DeleteDC(hdc);

	m_grid_mouse_on.x = grid.x;
	m_grid_mouse_on.y = grid.y;

	return S_OK;
}

LRESULT CBoxmanView::OnMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;

	if (m_h_info_dlg != NULL && ::IsWindowVisible(m_h_info_dlg)) return S_OK;
	
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	POINT grid = m_ui_assist->mouse_point_to_grid(&pt, &m_origin);
	if (grid.x == -1) {
		return S_OK;
	}

	if (_g_game_mode == (GAME_MODE) PLAY_GAME_MODE) {
		::SendMessage(m_h_mainframe, WM_WALK_TO_HERE, NULL, (LPARAM) &grid);	
	}

	if (_g_game_mode == (GAME_MODE) PLAY_RECORD_MODE) {
	}

	if (_g_game_mode == (GAME_MODE) EDIT_MAP_MODE) {
		m_ui_assist->add_element_to_grid(grid.y, grid.x);
		draw_grid(grid.y, grid.x, TRUE);
		::SendMessage(m_h_mainframe, WM_DISABLE_MAP_SAVE, NULL, NULL);
	}
	return S_OK;
}

LRESULT CBoxmanView::OnMouseRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	if (m_h_info_dlg != NULL && ::IsWindowVisible(m_h_info_dlg)) return S_OK;

	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	POINT grid = m_ui_assist->mouse_point_to_grid(&pt, &m_origin);
	if (grid.x == -1)
		return S_OK;

	if (_g_game_mode == (GAME_MODE) PLAY_GAME_MODE
		|| _g_game_mode == (GAME_MODE) PLAY_RECORD_MODE )
	{
	}
	else
	{
		erase_element(grid.y, grid.x);
		::SendMessage(m_h_mainframe, WM_DISABLE_MAP_SAVE, NULL, NULL);
	}

	return S_OK;
}

void CBoxmanView::draw_map_area(HDC hdc)
{
	RECT rc		= {0, 0, m_map_size.cx, m_map_size.cy};
	//RECT rc1	= {0, 0, m_info_size.cx, m_info_size.cy};

	m_ui_assist->transfer_coordinate(&rc, 1, &m_origin, FALSE);
	//m_ui_assist->transfer_coordinate(&rc1, 1, &m_info_origin, FALSE);
	
	HGDIOBJ hold = ::SelectObject(hdc, m_pen);
	draw_rect(hdc, &rc);
	//draw_rect(hdc, &rc1);
	::SelectObject(hdc, hold);
}

void CBoxmanView::draw_rect(HDC hdc, POINT *pt)
{
	::Polyline(hdc, pt, 5);
}

void CBoxmanView::draw_rect(HDC hdc, RECT *rc)
{
	POINT pt[5]= {
		{rc->left, rc->top}, 
		{rc->right, rc->top},
		{rc->right, rc->bottom},
		{rc->left, rc->bottom},
		{rc->left, rc->top - 1} //rc->top -1: make top-left corner is full
	};

	draw_rect(hdc, pt);
}

LRESULT CBoxmanView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	SIZE s;
	s.cx = rc.right - rc.left - 2 * m_origin.x;
	s.cy = rc.bottom - rc.top - 2 * m_origin.y;
	m_map_size.cx	= s.cx;

	//m_map_size.cx	= s.cx * 3 / 4;
	m_map_size.cy	= s.cy;
	m_info_size.cx	= s.cx * 1 / 4 - 4;
	m_info_size.cy	= s.cy;
	m_info_origin.x = m_origin.x + m_map_size.cx + 4;
	m_info_origin.y = m_origin.y;
	m_ui_assist->set_map_in_rect(&m_map_size, &m_origin);
	//layout();
	return S_OK;
}

void CBoxmanView::set_ui_assist(CUIAssist* ui_assist)
{
	m_ui_assist = ui_assist;
}

BOOL CBoxmanView::draw_grid(INT offset, BOOL b_create_dc)
{
	POINT grid = m_ui_assist->index_to_grid(offset);
	return draw_grid(grid.y, grid.x, b_create_dc);
}

BOOL CBoxmanView::draw_grid(INT row, INT col, BOOL b_create_dc)
{
	RECT rc;
	m_ui_assist->get_grid_rect(row, col, &rc);
	if (b_create_dc)
		m_ui_assist->transfer_coordinate(&rc, 1, &m_origin);
	
	INT width = rc.right - rc.left;
	INT height = rc.bottom - rc.top;

	if (width == 0)
		return TRUE;

	HDC hdc, hmemdc;
	if (b_create_dc) {
		hdc = ::GetDC(m_hWnd);
		hmemdc = ::CreateCompatibleDC(hdc);
	} else {
		hdc = m_mem_hdc_operator; // When WM_PAINT, first draw map in mem DC, then copy map to window DC.
		hmemdc = m_mem_hdc;
	}
	
	HBITMAP hbmp = m_ui_assist->get_bitmap_handle(row, col);
	HBITMAP hbgbmp = m_ui_assist->get_bitmap_from_snap(SNAP_BACKGROUND);
	if (hbmp != hbgbmp)
	{
		HBITMAP holdbmp = (HBITMAP)::SelectObject(hmemdc, hbmp);
		if (width == 32)
			::BitBlt(hdc, rc.left, rc.top, width, height, hmemdc, 0, 0, SRCCOPY);
		else
			::StretchBlt(hdc, rc.left, rc.top, width, height, hmemdc, 0, 0, 32, 32, SRCCOPY);
	}
	
	if (_g_game_mode == (GAME_MODE) EDIT_MAP_MODE)
	{
		HGDIOBJ hold = ::SelectObject(hdc, m_grid_pen);
		m_ui_assist->enlarge_rect(&rc, -1);
		draw_rect(hdc, &rc);
		::SelectObject(hdc, hold);
	}
	if (b_create_dc) {
		::DeleteDC(hdc);
		::DeleteDC(hmemdc);
	}

	return TRUE;	
}

BOOL CBoxmanView::draw_map()
{
	draw_bg(m_mem_hdc_operator);

	for (INT i = 0; i < m_ui_assist->get_grids().cx; i++)
	{
		for (INT j = 0; j < m_ui_assist->get_grids().cy; j++)
		{
			draw_grid(j, i, FALSE);
		}
	}
	
	/************************************************************************/
	/* copy map from memory DC to window DC
	/************************************************************************/
	::BitBlt(m_hdc, m_origin.x, m_origin.y, m_map_size.cx, m_map_size.cy, m_mem_hdc_operator, 0, 0, SRCCOPY);

	return TRUE;	
}

void CBoxmanView::remove_focus(INT row, INT col)
{
	RECT rc;
	m_ui_assist->get_grid_rect(row, col, &rc);
	m_ui_assist->transfer_coordinate(&rc, 1, &m_origin);
	m_ui_assist->enlarge_rect(&rc, 0);
	::InvalidateRect(m_hWnd, &rc, FALSE);
}

void CBoxmanView::draw_focus(HDC hdc, INT row, INT col)
{
	RECT rc;
	m_ui_assist->get_grid_rect(row, col, &rc);
	m_ui_assist->transfer_coordinate(&rc, 1, &m_origin);	
	HGDIOBJ hold = ::SelectObject(hdc, m_grid_focus_pen);
	m_ui_assist->enlarge_rect(&rc, -1);
	draw_rect(hdc, &rc);
	::SelectObject(hdc, hold);
}

void CBoxmanView::erase_element(INT row, INT col)
{
	RECT rc;
	m_ui_assist->delete_element_from_grid(row, col);
	m_ui_assist->get_grid_rect(row, col, &rc);
	m_ui_assist->transfer_coordinate(&rc, 1, &m_origin);
	::InvalidateRect(m_hWnd, &rc, TRUE);
}

void CBoxmanView::draw_element(INT row, INT col)
{
	m_ui_assist->add_element_to_grid(row, col);
	draw_grid(row, col, TRUE);
}

void CBoxmanView::set_main_frame(HWND hwnd)
{
	m_h_mainframe = hwnd;
}

LRESULT CBoxmanView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_static_info.Create(m_hWnd, NULL, NULL, WS_CHILD/*, WS_EX_TRANSPARENT*/);
	m_static_tips.Create(m_hWnd, NULL, NULL, WS_CHILD/*, WS_EX_TRANSPARENT*/);
	m_static_info.SetFont(m_hfont_info);
	m_static_tips.SetFont(m_hfont_info);
	m_static_info.SetWindowText(m_info);
	m_static_info.ShowWindow(SW_SHOW);
	m_static_tips.ShowWindow(SW_SHOW);
	
	return S_OK;
}

void CBoxmanView::layout()
{
	RECT rc_info = {m_info_origin.x, 
					m_info_origin.y, 
					m_info_origin.x + m_info_size.cx, 
					m_info_origin.y + m_info_size.cy / 3};
	RECT rc_tips = {m_info_origin.x, 
					rc_info.bottom - 2, 
					m_info_origin.x + m_info_size.cx, 
					m_info_origin.y + m_info_size.cy};
	
	m_ui_assist->enlarge_rect(&rc_info, -1);
	m_ui_assist->enlarge_rect(&rc_tips, -1);
	m_static_info.MoveWindow(&rc_info);
	m_static_tips.MoveWindow(&rc_tips);
}

void CBoxmanView::set_tips()
{
	if (_g_game_mode == (GAME_MODE) PLAY_GAME_MODE 
		|| _g_game_mode == (GAME_MODE) PLAY_RECORD_MODE )
		m_static_tips.SetWindowText(m_tips_game);
	else
		m_static_tips.SetWindowText(m_tips_map);
}

void CBoxmanView::draw_bg(HDC hdc)
{
	HBRUSH hb;
	if (_g_game_mode == (GAME_MODE) PLAY_GAME_MODE
		|| _g_game_mode == (GAME_MODE) PLAY_RECORD_MODE )
		hb = ::CreatePatternBrush(m_ui_assist->get_bitmap_from_snap(SNAP_BACKGROUND));
	else
		hb = ::CreateSolidBrush(RGB(0xff, 0xff, 0xff));

	HGDIOBJ hold = ::SelectObject(hdc, hb);
	::PatBlt(hdc, 0, 0, m_map_size.cx, m_map_size.cy, PATCOPY);
	::SelectObject(hdc, hold);
	::DeleteObject(hb);
}

void CBoxmanView::get_map_rect(RECT* rc)
{
	rc->left	= m_origin.x;
	rc->top		= m_origin.y;
	rc->right	= rc->left + m_map_size.cx;
	rc->bottom	= rc->top + m_map_size.cy;
}

void CBoxmanView::refresh_map_area()
{
	RECT rc;
	get_map_rect(&rc);
	::InvalidateRect(m_hWnd, &rc, TRUE);
}

void CBoxmanView::out_info(LPTSTR info)
{
	int len = lstrlen(m_info);
	wsprintf(m_info + len, "\r\n%s", info);
	m_static_info.SetWindowText(m_info);
	*((CHAR*) m_info + len) = '\0';	
}

void CBoxmanView::init_font()
{
	LOGFONT lf;
    lf.lfHeight		= 16;
    lf.lfWidth		= 8;
    lf.lfEscapement	= 0;
    lf.lfOrientation= 0;
    lf.lfWeight		= 400;              
    lf.lfItalic		= FALSE;              
    lf.lfUnderline		= FALSE;
    lf.lfStrikeOut		= FALSE;
    lf.lfCharSet		= GB2312_CHARSET;
    lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality		= DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH;
    lstrcpy( lf.lfFaceName, _T("ËÎÌו"));
	m_hfont_info = ::CreateFontIndirect(&lf);
}

void CBoxmanView::set_infodlg_handle(HWND hwnd)
{
	m_h_info_dlg = hwnd;
}

void CBoxmanView::load_tips()
{
	ZeroMemory(m_tips_game, 512);
	ZeroMemory(m_tips_map, 512);

	TCHAR sz_instruction[32]		= {0};
	TCHAR sz_game_back[128]			= {0};
	TCHAR sz_game_forward[128]		= {0};
	TCHAR sz_game_record_tips[128]	= {0};
	::LoadString(_Module.GetResourceInstance(), IDS_INSTRUCTION, sz_instruction, 32);
	::LoadString(_Module.GetResourceInstance(), IDS_GAME_BACK, sz_game_back, 128);
	::LoadString(_Module.GetResourceInstance(), IDS_GAME_FORWARD, sz_game_forward, 128);
	::LoadString(_Module.GetResourceInstance(), IDS_GAME_RECORD_TIPS, sz_game_record_tips, 128);
	wsprintf(m_tips_game, "%s\r\n\r\n%s\r\n\r\n%s\r\n\r\n%s", 
							sz_instruction, 
							sz_game_back, 
							sz_game_forward, 
							sz_game_record_tips);
	
	TCHAR sz_map_draw_single[128]		= {0};
	TCHAR sz_map_draw_continuious[128]	= {0};
	TCHAR sz_map_erase_single[128]		= {0};
	TCHAR sz_map_erase_continuious[128]	= {0};
	::LoadString(_Module.GetResourceInstance(), IDS_MAP_TIP_DRAW_SINGLE, sz_map_draw_single, 128);
	::LoadString(_Module.GetResourceInstance(), IDS_MAP_TIP_DRAW_CONTINUIOUS, sz_map_draw_continuious, 128);
	::LoadString(_Module.GetResourceInstance(), IDS_MAP_TIP_ERASE_SINGLE, sz_map_erase_single, 128);
	::LoadString(_Module.GetResourceInstance(), IDS_MAP_TIP_ERASE_CONTINUIOUS, sz_map_erase_continuious, 128);
	wsprintf(m_tips_map, "%s\r\n\r\n%s\r\n\r\n%s\r\n\r\n%s\r\n\r\n%s", 
							sz_instruction, 
							sz_map_draw_single, 
							sz_map_draw_continuious, 
							sz_map_erase_single, 
							sz_map_erase_continuious);

	::LoadString(_Module.GetResourceInstance(), IDS_INFO, m_info, 256);
}
