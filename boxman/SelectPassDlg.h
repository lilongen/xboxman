// SelectPassDlg.h: interface for the CSelectPassDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELECTPASSDLG_H__29407343_1732_45B9_98D8_EA632EF07591__INCLUDED_)
#define AFX_SELECTPASSDLG_H__29407343_1732_45B9_98D8_EA632EF07591__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <atlctrls.h>

class CSelectPassDlg : public CDialogImpl<CSelectPassDlg>
{
public:
	enum { IDD = IDD_SELECT_PASS };

	BEGIN_MSG_MAP(CSelectPassDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		COMMAND_ID_HANDLER(IDC_OK, OnOK)
		COMMAND_ID_HANDLER(IDC_CANCEL, OnClose)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	INT		get_pass_no();

private:
	CEdit m_pass_no;
	CUpDownCtrl m_up_down;
	TCHAR m_sz[16];
};

#endif // !defined(AFX_SELECTPASSDLG_H__29407343_1732_45B9_98D8_EA632EF07591__INCLUDED_)
