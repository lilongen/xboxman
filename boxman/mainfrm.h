// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__C24494E0_2E34_49CC_BD5A_9E7BCD7E8BD4__INCLUDED_)
#define AFX_MAINFRM_H__C24494E0_2E34_49CC_BD5A_9E7BCD7E8BD4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include "BoxmanView.h"
#include "Map.h"
#include "UIAssist.h"
#include "History.h"
#include "InfoDlg.h"
#include "MapExImporter.h"
#include "PathAI.h"

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
	
		BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_DISABLE_MAP_SAVE, OnDisableMapSave)
		MESSAGE_HANDLER(WM_WALK_TO_HERE, OnWalkto)
		COMMAND_ID_HANDLER(ID_GAME_START, OnStart)
		COMMAND_ID_HANDLER(ID_GAME_LOAD, OnLoad)
		COMMAND_ID_HANDLER(ID_EDIT_MAP_IMPORT, import_maps);
		COMMAND_ID_HANDLER(ID_TB_GAME_PLAY, OnPlay)
		COMMAND_ID_HANDLER(ID_EDIT_MAP_NEW, OnEditNewMap)
		COMMAND_ID_HANDLER(ID_EDIT_MAP, OnEditMap)

		COMMAND_ID_HANDLER(ID_APP_EXIT, OnExit)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_RANGE_HANDLER(ID_MAP_MAN, ID_MAP_VERIFY, OnChooseMapElement)
		COMMAND_RANGE_HANDLER(ID_GAME_BACK, ID_GAME_PAUSE, OnGameControl)
		COMMAND_RANGE_HANDLER(ID_FORCE_LEVEL_1, ID_FORCE_NO_LIMIT, OnForceSet)
		
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()


// Handler prototypes (uncomment arguments if needed):
//s	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CMainFrame();
	virtual ~CMainFrame();
	virtual BOOL OnIdle();
	
private:
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDisableMapSave(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnWalkto(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnLoad(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnEditMap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnEditNewMap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT import_maps(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnChooseMapElement(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnGameControl(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnForceSet(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	BOOL	get_affected_grids(INT step, OUT INT* changes, OUT POINT* pt, OUT INT& affected_cnt);
	void	change_data(INT* changes, POINT* pt, INT n_changes_cnt);
	void	go_history(INT offset /* 1: forward, -1: go back*/);
	void	enable_map_save(BOOL enable);
	void	play_next();

	void	ShowToolBar();
	BOOL	handle_key_down(INT step);
	void	set_statusbar_text(INT ids);
	void	set_statusbar_text(LPTSTR str);
	INT		get_ids_from_ctrl(INT ctrl_id);
	BOOL	save_history();
	BOOL	play_history();
	BOOL	open_history();
	void	set_tb_game_ui();
	void	refresh_ui();
	void	reset_opened_history();
	void	handle_pass_finished();
	void	load_data_and_start();

	void	show_info_dlg(BOOL showit);
	INT		get_player_pass_no(LPTSTR player = NULL);
	BOOL	write_player_pass_no(LPTSTR player = NULL);
	BOOL	start_game();
	void	show_no_map_info();

	INT		direction2Step(UINT direction);
	UINT    step2Direction(INT step);

public:
	CBoxmanView			m_view;
	CCommandBarCtrl		m_CmdBar;
	CUIAssist			m_ui_assist;
	CMap				m_map;
	HWND				m_h_tb_map;
	HWND				m_h_tb_game;
	INT					m_checked_tb_button;
	CHistory			m_history;
	HWND				m_hwnd_info;
	BOOL				m_play_satus; //TRUE: play status, FALSE: pause status
	POINT*				m_ctrl2tip_ids;
	BOOL				m_history_opened;
	BOOL				m_pass_finished;
	CInfoDlg			m_info_dlg;
	CPathAI*			m_p_explorer;
	
	INT					m_man_force_level;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__C24494E0_2E34_49CC_BD5A_9E7BCD7E8BD4__INCLUDED_)
