// boxmanView.h : interface of the CBoxmanView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOXMANVIEW_H__0F45F0EA_4AD1_4499_8499_F7317D397238__INCLUDED_)
#define AFX_BOXMANVIEW_H__0F45F0EA_4AD1_4499_8499_F7317D397238__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "UIAssist.h"
#define WM_DISABLE_MAP_SAVE (WM_USER + 2005)
#define WM_WALK_TO_HERE (WM_USER + 2007)

#include <atlctrls.h>
class CBoxmanView : public CWindowImpl<CBoxmanView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(CBoxmanView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMouseLButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnMouseRButtonDown)
	END_MSG_MAP()
	
	CBoxmanView();
	virtual ~CBoxmanView();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BOOL	draw_grid(INT row, INT col, BOOL b_create_dc);
	BOOL	draw_grid(INT offset, BOOL b_create_dc);
	BOOL	draw_map();
	void	set_ui_assist(CUIAssist* ui_assist);
	void	set_main_frame(HWND hwnd);
	void	set_tips();
	void	get_map_rect(RECT* rc);
	void	refresh_map_area();
	void	out_info(LPTSTR info);
	void	set_infodlg_handle(HWND hwnd);

private:
	void draw_map_area(HDC hdc);
	void draw_rect(HDC hdc, POINT *pt);
	void draw_rect(HDC hdc, RECT *rc);
	void remove_focus(POINT* pt);
	void remove_focus(INT row, INT col);
	void draw_focus(HDC hdc, INT row, INT col);
	void erase_element(INT row, INT col);
	void draw_element(INT row, INT col);
	void layout();
	void draw_bg(HDC hdc);
	void init_font();
	void load_tips();
	
private:
	POINT	m_origin;
	POINT	m_info_origin;
	HDC		m_hdc;
	HDC		m_mem_hdc;
	HDC		m_mem_hdc_operator;
	SIZE	m_map_size;
	SIZE	m_info_size;
	CUIAssist* m_ui_assist;
	
	POINT	m_grid_mouse_on;
	HPEN	m_grid_focus_pen;
	HPEN	m_grid_pen;
	HPEN	m_pen;

	HWND	m_h_mainframe;
	CStatic m_static_tips;
	CStatic m_static_info;
	TCHAR	m_info[256];

	HFONT	m_hfont_info;
	HWND	m_h_info_dlg;

	TCHAR	m_tips_game[512];
	TCHAR	m_tips_map[512];
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOXMANVIEW_H__0F45F0EA_4AD1_4499_8499_F7317D397238__INCLUDED_)
