// SelectPass.cpp: implementation of the CSelectPassDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SelectPassDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
WNDPROC p_old;

inline INT get_pass_no(HWND hwnd)
{
	TCHAR text[16] = {0};
	::SendMessage(hwnd, WM_GETTEXT, 16, LPARAM(text));

	INT val = atoi(text);
	return val;
}

LRESULT CALLBACK pass_edit_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg != WM_CHAR)
		return p_old(hwnd, uMsg, wParam, lParam);
	
	INT visible_amount = _g_pass_amount - 1;
	if (_g_game_mode == (GAME_MODE) PLAY_GAME_MODE && _g_player_passed_amount < _g_pass_amount)
		visible_amount = _g_player_passed_amount;
	if (wParam >= '0' && wParam <= '9')
	{
		INT value = get_pass_no(hwnd) * 10 + INT(wParam) - INT('0');
		if (value > visible_amount + 1)
			return S_OK;
		else
			return p_old(hwnd, uMsg, wParam, lParam);
	}
	else if (wParam == VK_RIGHT || wParam == VK_LEFT || wParam == VK_BACK)
	{
		return p_old(hwnd, uMsg, wParam, lParam);
	}

	return S_OK;

}

LRESULT CSelectPassDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	m_pass_no.Attach(GetDlgItem(IDC_EDIT_PASS_NO));
	m_up_down.Attach(GetDlgItem(IDC_SPIN_PASS_NO));
	p_old = (WNDPROC) m_pass_no.SetWindowLong(GWL_WNDPROC, (LONG) pass_edit_proc);
	m_pass_no.SetWindowText(_T("1"));

	TCHAR sz[64] = {0};
	GetDlgItemText(IDC_STATIC_TIPS, sz, 64);
	wsprintf(sz, "%s %d", sz, _g_pass_amount);
	SetDlgItemText(IDC_STATIC_TIPS, sz);

	return TRUE;
}

LRESULT CSelectPassDlg::OnClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return -1;
}

LRESULT CSelectPassDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	_g_pass_no = get_pass_no() - 1;
	EndDialog(wID);
	return 0;
}

LRESULT CSelectPassDlg::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam != IDC_SPIN_PASS_NO)
		return S_OK;
	
	INT visible_amount = _g_pass_amount - 1;
	if (_g_game_mode == (GAME_MODE) PLAY_GAME_MODE && _g_player_passed_amount < _g_pass_amount)
		visible_amount = _g_player_passed_amount;
	
	LPNMUPDOWN ud = LPNMUPDOWN(lParam);
	INT no = get_pass_no();
	no += - ud->iDelta;
	if (no < 1 || no > visible_amount + 1)
		return S_OK;
	
	TCHAR value[16] = {0};
	wsprintf(value, "%d", no);
	m_pass_no.SetWindowText(value);
	
	return S_OK;
}

INT CSelectPassDlg::get_pass_no()
{
	INT no = - 1;
	m_pass_no.GetWindowText(m_sz, 16);
	no = atoi(m_sz);

	return no;
}
