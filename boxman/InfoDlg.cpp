// InfoDlg.cpp: implementation of the CInfoDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InfoDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInfoDlg::CInfoDlg()
{
	ZeroMemory(m_info, 256);
}

CInfoDlg::~CInfoDlg()
{

}

void CInfoDlg::set_info(LPTSTR info)
{
	INT len = max(256, lstrlen(info));
	lstrcpyn(m_info, info, len);
	SetDlgItemText(IDC_STATIC_INFO, m_info);
}

LRESULT CInfoDlg::OnLButtonDBCLK(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ShowWindow(SW_HIDE);
	::SetCursorPos(m_cursor_point.x, m_cursor_point.y);
	return S_OK;
}

LRESULT CInfoDlg::OnInitDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	TCHAR tips[128]= {0};
	::LoadString(_Module.GetResourceInstance(), IDS_CLOSE_INFO_DLG_TIPS, tips, 128);
	SetDlgItemText(IDC_STATIC_TIPS, tips);

	return S_OK;
}

void CInfoDlg::set_cursor_point(POINT pt)
{
	m_cursor_point.x = pt.x;
	m_cursor_point.y = pt.y;
}
