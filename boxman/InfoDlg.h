// InfoDlg.h: interface for the CInfoDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFODLG_H__8AA7D693_7D74_4BC8_BF2F_C4341F7E6718__INCLUDED_)
#define AFX_INFODLG_H__8AA7D693_7D74_4BC8_BF2F_C4341F7E6718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInfoDlg : public CDialogImpl<CInfoDlg>
{
public:
	enum { IDD = IDD_INFO };

	BEGIN_MSG_MAP(CInfoDlg)
	MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDBCLK)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDlg)
	END_MSG_MAP()
	
	CInfoDlg();
	~CInfoDlg();
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
public:
	void set_info(LPTSTR info);
	void set_cursor_point(POINT pt);

private:
	TCHAR m_info[256];
	LRESULT OnLButtonDBCLK(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	POINT m_cursor_point;
};

#endif // !defined(AFX_INFODLG_H__8AA7D693_7D74_4BC8_BF2F_C4341F7E6718__INCLUDED_)
