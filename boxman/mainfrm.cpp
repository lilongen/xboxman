#include "stdafx.h"
#include "mainfrm.h"
#include "Aboutdlg.h"
#include "SelectPassDlg.h"

#define TB_MAP 0
#define TB_GAME 1
#define HOTKEY_BACK 0x2005
#define HOTKEY_FORWARD 0x2006
#define PLAY_TIMER_ID	223241
#define PLAY_INTERVAL 150
void get_full_path(LPTSTR sz_path, INT length, LPTSTR file_name = 0);

LRESULT CMainFrame::OnDisableMapSave(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	enable_map_save(FALSE);
	return S_OK;
}

LRESULT CMainFrame::OnWalkto(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	POINT* destination = (POINT*) lParam;
	if (m_map.isBarrier(m_map.grid_to_index(destination->y, destination->x))) {
		return S_OK;
	}
	
	m_p_explorer->setDestination(*destination);

#ifdef _DEBUG
TCHAR szDebug[128] = {0};
wsprintf(szDebug, "destination.x %02d, destination.y: %02d\n", destination->x, destination->y);
OutputDebugString(szDebug);
wsprintf(szDebug, "Origin.x %02d, Origin.y: %02d\n---------------------------\n", m_p_explorer->getOrigin().x, m_p_explorer->getOrigin().y);
OutputDebugString(szDebug);
#endif

	std::vector <INT>& paths = m_p_explorer->getPaths();
	if (paths.size() < 1) {
		return S_OK;
	}
	
	INT VK = -1;
	INT direction = 0;
	for (std::vector <INT>::reverse_iterator vi = paths.rbegin() + 1; vi != paths.rend(); vi++) {
		direction = *vi - *(vi - 1);
		
#ifdef _DEBUG
POINT pt1 = m_map.index_to_grid(*(vi - 1));
wsprintf(szDebug, "(%02d, %02d)->", pt1.x, pt1.y);
OutputDebugString(szDebug);
#endif
		
		handle_key_down(direction);
		Sleep(100);
	}

#ifdef _DEBUG
wsprintf(szDebug, "(%02d, %02d)", destination->x, destination->y);
OutputDebugString(szDebug);
#endif
	
	return S_OK;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;
	
	if (pMsg->message == WM_KEYDOWN) {
		if (_g_game_mode == GAME_MODE(PLAY_RECORD_MODE) ||
			_g_game_mode == GAME_MODE(EDIT_MAP_MODE)) {
			return TRUE;
		}

		if (pMsg->wParam == VK_RETURN && m_pass_finished) {
			show_info_dlg(FALSE);
			play_next();
			m_pass_finished = FALSE;
			return TRUE;
		}

		if (pMsg->wParam == VK_ESCAPE) {
			show_info_dlg(FALSE);
			return TRUE;
		}
		
		if (pMsg->wParam == VK_DOWN
			|| pMsg->wParam == VK_UP
			|| pMsg->wParam == VK_LEFT
			|| pMsg->wParam == VK_RIGHT) {
			
			if (!m_pass_finished) {
				BOOL ret = handle_key_down(direction2Step(pMsg->wParam));
				
				return TRUE;
			} else {
				BOOL visible = ::IsWindowVisible(m_info_dlg.m_hWnd);
				if (!visible) {
					TCHAR tips[128] = {0};
					::LoadString(_Module.GetResourceInstance(), IDS_GAME_CONTINUE_TIPS, tips, 128);
					m_info_dlg.set_info(tips);
					show_info_dlg(TRUE);

					return TRUE;
				}
			}
		}
	}

	if (pMsg->message == WM_HOTKEY) {
		if (LOWORD(pMsg->lParam) == HOTKEYF_CONTROL) {
			if (HIWORD(pMsg->lParam) == 'Z')
				go_history(-1);
			else if (HIWORD(pMsg->lParam) == 'Y')
				go_history(1);

		}
		return TRUE;
	}

	return m_view.PreTranslateMessage(pMsg);
}

CMainFrame::CMainFrame()
{
	m_ctrl2tip_ids = new POINT[16];
	POINT map[] = {
		{ID_MAP_MAN,			IDS_MAN}, 
		{ID_MAP_BOX,			IDS_BOX}, 
		{ID_MAP_FLOOR,			IDS_FLOOR}, 
		{ID_MAP_DESTINATION,	IDS_DESTINATION}, 
		{ID_MAP_WALL,			IDS_WALL}, 
		{ID_MAP_SAVE,			IDS_MAP_SAVE}, 
		{ID_MAP_VERIFY,			IDS_MAP_VERIFY}, 
		
		{ID_GAME_BACK,			IDS_GAME_BACK}, 
		{ID_GAME_FORWARD,		IDS_GAME_FORWARD}, 
		{ID_GAME_SAVE,			IDS_GAME_SAVE}, 
		{ID_GAME_OPEN,			IDS_GAME_OPEN}, 
		{ID_GAME_PLAY,			IDS_GAME_PLAY}, 
		{ID_GAME_STOP,			IDS_GAME_STOP}, 
	};
	memcpy(m_ctrl2tip_ids, map, sizeof(map));
	m_play_satus = FALSE;
	m_history_opened = FALSE;
	m_pass_finished = FALSE;
	m_man_force_level = 1;
	m_p_explorer = NULL;
}

INT CMainFrame::get_ids_from_ctrl(INT ctrl_id)
{
	for (INT i = 0; i < 16 && m_ctrl2tip_ids[i].x > 0; i ++)
	{
		if (m_ctrl2tip_ids[i].x == ctrl_id)
			return m_ctrl2tip_ids[i].y;
	}

	return 0;
}

CMainFrame::~CMainFrame()
{	
	if (NULL != m_p_explorer) {
		delete m_p_explorer;
		m_p_explorer = NULL;
	}

	if (NULL != m_info_dlg.m_hWnd)
		m_info_dlg.DestroyWindow();


	delete[] m_ctrl2tip_ids;
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	::MoveWindow(m_hWnd, 280, 180, 750, 580, 1);

	return 0;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	_g_game_mode = (GAME_MODE) PLAY_GAME_MODE;
	m_ui_assist.set_map(&m_map);
	m_view.set_ui_assist(&m_ui_assist);
	m_view.set_infodlg_handle(m_info_dlg.m_hWnd);
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	//m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar_map = CreateSimpleToolBarCtrl(m_hWnd, IDR_TB_EDIT_MAP, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	HWND hWndToolBar_game = CreateSimpleToolBarCtrl(m_hWnd, IDR_TB_GAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	m_h_tb_map = hWndToolBar_map;
	m_h_tb_game = hWndToolBar_game;
	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar_map, NULL, TRUE);
	AddSimpleReBarBand(hWndToolBar_game, NULL, TRUE);
	CreateSimpleStatusBar();
	
	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_view.set_main_frame(m_hWnd);
	m_info_dlg.Create(m_hWnd);
	
	UIAddToolBar(hWndToolBar_map);
	set_tb_game_ui();
	ShowToolBar();
	POINT pt = {5, 5};
	::ClientToScreen(m_view.m_hWnd, &pt);
	m_info_dlg.set_cursor_point(pt);

	// register object for message filtering and idle updates
	::RegisterHotKey(m_hWnd, HOTKEY_BACK, MOD_CONTROL, 'Z');
	::RegisterHotKey(m_hWnd, HOTKEY_FORWARD, MOD_CONTROL, 'Y');
	_g_player_passed_amount = get_player_pass_no(NULL);
	start_game();
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
	
	return 0;
}

LRESULT CMainFrame::OnExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	::UnregisterHotKey(m_hWnd, HOTKEY_BACK);
	::UnregisterHotKey(m_hWnd, HOTKEY_FORWARD);

	PostMessage(WM_CLOSE);
	return 0;
}

void CMainFrame::ShowToolBar()
{
	CReBarCtrl rebar = m_hWndToolBar;
	int nTBMapBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	int nTBGameBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 2);	// toolbar is 2nd added band
	
	if (_g_game_mode == (GAME_MODE) EDIT_MAP_MODE)
	{
		rebar.ShowBand(nTBGameBandIndex, FALSE);
		rebar.ShowBand(nTBMapBandIndex, TRUE);
		::SendMessage(m_h_tb_map, WM_COMMAND, ID_MAP_WALL, NULL);
		enable_map_save(FALSE);
	}
	else 
	{
		rebar.ShowBand(nTBGameBandIndex, TRUE);
		rebar.ShowBand(nTBMapBandIndex, FALSE);
		::SendMessage(m_h_tb_game, TB_ENABLEBUTTON, ID_GAME_SAVE, FALSE);
		::SendMessage(m_h_tb_game, TB_HIDEBUTTON, ID_GAME_PAUSE, LPARAM(TRUE));
	}
	set_statusbar_text(_T(""));
	UpdateLayout();
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

INT CMainFrame::direction2Step(UINT direction) {
	INT step = 0;
	switch(direction) {
	case VK_UP:
		step = -m_map.m_p_ph->cols;
		break;
	case VK_DOWN:
		step = m_map.m_p_ph->cols;
		break;
	case VK_LEFT:
		step = - 1;
		break;
	case VK_RIGHT:
		step = 1;
		break;
	}

	return step;
}

UINT CMainFrame::step2Direction(INT step) {
	if (step == -m_map.m_p_ph->cols)	return VK_UP;
	if (step == m_map.m_p_ph->cols)		return VK_DOWN;
	if (step == -1)						return VK_LEFT;
	if (step == 1)						return VK_RIGHT;

	return 0;
}

BOOL CMainFrame::handle_key_down(INT step)
{
	m_ui_assist.set_vk(step2Direction(step));
	
	INT *changes = new INT[max(m_map.m_p_ph->cols, m_map.m_p_ph->rows)];
	POINT *pt = new POINT[max(m_map.m_p_ph->cols, m_map.m_p_ph->rows)];	
	INT affected_cnt;
	m_history.reset_length();
	BOOL ret = get_affected_grids(step, OUT changes, OUT pt, OUT affected_cnt);
	
	if (affected_cnt > 0)
	{
		if (affected_cnt > 2)
			m_view.draw_grid(pt[affected_cnt - 1].y, pt[affected_cnt - 1].x, TRUE);
		m_view.draw_grid(pt[1].y, pt[1].x, TRUE);
		m_view.draw_grid(pt[0].y, pt[0].x, TRUE);
	}

	TCHAR sz[64] = {0};
	::LoadString(_Module.GetResourceInstance(), IDS_GAME_STEPS, sz, 32);
	wsprintf(sz, "%s %d", sz, m_history.get_length());
	set_statusbar_text(sz);
	if (m_map.is_passed())
	{
		handle_pass_finished();
	}
	
	delete[] changes;
	delete[] pt;

	return ret;
}

LRESULT CMainFrame::OnStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	start_game();
	
	return 0;
}

LRESULT CMainFrame::OnLoad(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (_g_pass_amount < 1)
	{
		show_no_map_info();
		return S_OK;
	}
	
	GAME_MODE old = _g_game_mode;
	_g_game_mode = (GAME_MODE) PLAY_GAME_MODE; 
	CSelectPassDlg dlg_select;
	INT ret = dlg_select.DoModal();
	if (ret == IDC_CANCEL)
	{
		_g_game_mode = old;
		return S_FALSE;
	}
		
	_g_game_mode = (GAME_MODE) PLAY_GAME_MODE;
	load_data_and_start();

	return S_OK;
}

LRESULT CMainFrame::OnEditMap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (_g_pass_amount < 1)
	{
		show_no_map_info();
		return S_OK;
	}
	
	GAME_MODE old = _g_game_mode;
	_g_game_mode = (GAME_MODE) EDIT_MAP_MODE;
	CSelectPassDlg dlg_select;
	INT ret = dlg_select.DoModal();
	if (ret == IDC_CANCEL)
	{
		_g_game_mode = old;
		return S_FALSE;
	}
	
	_g_game_mode = (GAME_MODE) EDIT_MAP_MODE;
	load_data_and_start();

	return S_OK;
}

LRESULT CMainFrame::OnEditNewMap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	_g_game_mode = (GAME_MODE) EDIT_MAP_MODE;
	m_pass_finished = FALSE;
	_g_pass_no = -1;
	m_history.init(FALSE);
	m_map.init();
	refresh_ui();

	ShowToolBar();
	return S_OK;
}

LRESULT CMainFrame::OnGameControl(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	switch(wID) {
	case ID_GAME_BACK:
		go_history(-1);
		break;
	case ID_GAME_FORWARD:
		go_history(1);
		break;
	case ID_GAME_SAVE:
		save_history();
		break;
	case ID_GAME_PLAY:
		m_play_satus = !m_play_satus;
		set_tb_game_ui();
		break;
	case ID_GAME_OPEN:
		open_history();
		break;
	case ID_GAME_STOP:
		reset_opened_history();
		break;			
	}

	return S_OK;
}

BOOL CMainFrame::save_history()
{
	TCHAR sz_file[256] = {0};
	wsprintf(sz_file, "pass_%d", _g_pass_no);
    
	OPENFILENAME Ofn;
    memset( &Ofn, 0, sizeof(OPENFILENAME));
    Ofn.lStructSize		= sizeof(OPENFILENAME); 
    Ofn.hwndOwner		= m_hWnd; 
	Ofn.lpstrFilter		= _T("Boaman Record File (*.xman)\0*.xman\0\0"); 
    Ofn.lpstrFile		= sz_file; 
    Ofn.nMaxFile		= 256; 
    Ofn.lpstrTitle		= _T("Save record to file");;
    Ofn.lpstrInitialDir = ( LPTSTR ) NULL; 
    Ofn.Flags = OFN_OVERWRITEPROMPT; 

    if (!::GetSaveFileName( &Ofn ))
        return FALSE;
	
	CHAR* suffix = (CHAR*) Ofn.lpstrFile + lstrlen(Ofn.lpstrFile) - lstrlen(".xman");
	if (strcmp(suffix, ".xman") != 0)
		wsprintf(Ofn.lpstrFile, "%s.xman", Ofn.lpstrFile);
	
	FILETIME* ft = m_map.get_map_last_modified(_g_pass_no);
	return m_history.save_to_file(Ofn.lpstrFile, ft);

}

BOOL CMainFrame::play_history()
{
	if (!m_history.is_play_over())
		go_history(1);
	else
		reset_opened_history();		
	return TRUE;
}

BOOL CMainFrame::open_history()
{
	TCHAR sz_file[256] = {0};
	OPENFILENAME Ofn;
    memset(&Ofn, 0, sizeof(OPENFILENAME));
    Ofn.lStructSize		= sizeof(OPENFILENAME); 
    Ofn.hwndOwner		= m_hWnd; 
	Ofn.lpstrFilter		= _T("Boaman Record File (*.xman)\0*.xman\0\0"); 
    Ofn.lpstrFile		= sz_file; 
    Ofn.nMaxFile		= 256; 
    Ofn.lpstrTitle		= _T("Open record file");;
    Ofn.lpstrInitialDir = ( LPTSTR ) NULL; 
    Ofn.Flags			= OFN_READONLY;
	
    if (!::GetOpenFileName(&Ofn))
        return FALSE;
	
	FILETIME ft;
	BOOL ret = m_history.load_from_file(Ofn.lpstrFile, &ft);
	if (!ret)
		return FALSE;
	
	FILETIME* pass_last_modified = m_map.get_map_last_modified(_g_pass_no);
	if (!(ft.dwHighDateTime == pass_last_modified->dwHighDateTime
		&& ft.dwLowDateTime == pass_last_modified->dwLowDateTime))
	{
		m_history.init(FALSE);
		TCHAR info[256] = {0};
		::LoadString(_Module.GetResourceInstance(), IDS_PLAY_PASS_UPDATED, info, 256);
		m_info_dlg.set_info(info);
		show_info_dlg(TRUE);

		return FALSE;
	}
	
	_g_game_mode = (GAME_MODE) PLAY_RECORD_MODE;
	m_map.load(_g_pass_no);
	m_ui_assist.reset_man_position();
	refresh_ui();
	SetTimer(PLAY_TIMER_ID, PLAY_INTERVAL);
	m_play_satus = TRUE;
	m_history_opened = TRUE;
	set_tb_game_ui();
	ShowToolBar();

	return TRUE;
}

LRESULT CMainFrame::OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (_g_pass_amount < 1)
	{
		show_no_map_info();
		return S_OK;
	}
	open_history();
	return S_OK;
}


LRESULT CMainFrame::OnChooseMapElement(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wID == ID_MAP_SAVE)
	{
		m_ui_assist.save();
		m_ui_assist.refresh();
		::InvalidateRect(m_view.m_hWnd, NULL, TRUE);
		m_map.save();
		return S_OK;
	}

	if (wID == ID_MAP_VERIFY)
	{
		BOOL ret = m_map.is_map_legal();
		TCHAR out[256] = {0};
		TCHAR info[128] = {0};
		TCHAR rule[128] = {0};
		INT info_id = ret ? IDS_MAP_IS_LEGAL : IDS_MAP_IS_NOT_LEGAL;
		::LoadString(_Module.GetResourceInstance(), info_id, info, 256);
		::LoadString(_Module.GetResourceInstance(), IDS_MAP_RULE, rule, 256);
		wsprintf(out, "%s\r\n%s", info, rule);
		m_info_dlg.set_info(out);
		show_info_dlg(TRUE);
		if (ret)
			enable_map_save(TRUE);
		
		return S_OK;
	}

	MAP_ELEMENT me;
	switch(wID) {
	case ID_MAP_MAN:
		me = MAN;
		break;
	case ID_MAP_BOX:
		me = BOX;
		break;
	case ID_MAP_FLOOR:
		me = FLOOR;
		break;
	case ID_MAP_DESTINATION:
		me = DESTINATION;
		break;
	case ID_MAP_WALL:
		me = WALL;
		break;
	default:
		me = FLOOR;
	}

	::SendMessage(m_h_tb_map, TB_CHECKBUTTON, m_checked_tb_button, FALSE);
	::SendMessage(m_h_tb_map, TB_CHECKBUTTON, wID, TRUE);
	m_checked_tb_button = wID;
	
	m_ui_assist.set_selected_element(me);
	INT ids = get_ids_from_ctrl(wID);
	TCHAR text[256] = {0};
	::LoadString(_Module.GetResourceInstance(), IDS_MAP_SELECTED, text, 128);
	::LoadString(_Module.GetResourceInstance(), ids, text + lstrlen(text), 128);
	set_statusbar_text(text);
	UpdateLayout();

	return S_OK;
}

void CMainFrame::enable_map_save(BOOL enable)
{
	::SendMessage(m_h_tb_map, TB_ENABLEBUTTON, ID_MAP_SAVE, enable);
}

BOOL CMainFrame::get_affected_grids(INT step, OUT INT* changes, OUT POINT* pt, OUT INT& affected_cnt)
{
	POINT* pt_man = m_ui_assist.get_man_position();
	INT man_offset = m_map.grid_to_index(pt_man->y, pt_man->x);
	INT cnt = m_map.m_p_ph->cols * m_map.m_p_ph->rows;
	if (man_offset + step >= cnt || man_offset + step < 0)
		return FALSE;
	
	BYTE val, h4bit, l4bit;
	affected_cnt = 1;
	changes[0] = man_offset;
	pt[0] = m_map.index_to_grid(changes[0]);
	for (INT i = man_offset + step; i >=0 && i < cnt; i += step) {
		val = m_map.get(i);
		h4bit = val / 16;
		l4bit = val % 16;
		if (l4bit == (MAP_ELEMENT) WALL || l4bit == NULL) {
			affected_cnt = 0;
			break;
		}
		
		changes[affected_cnt] = i;
		pt[affected_cnt] = m_map.index_to_grid(changes[affected_cnt]);
		affected_cnt ++;
		
		if (val < 16)
			break;
	}
	if (affected_cnt == 0 || affected_cnt > m_man_force_level + 2)
		return FALSE;
	
	change_data(changes, pt, affected_cnt);
	m_history.add(pt_man, step, affected_cnt);
	::SendMessage(m_h_tb_game, TB_ENABLEBUTTON, ID_GAME_SAVE, TRUE);
	return TRUE;
}

void CMainFrame::change_data(INT* changes, POINT* pt, INT affected_cnt)
{
	if 	(affected_cnt > 2)
	{
		m_map.set(changes[affected_cnt - 1], m_map.get(changes[affected_cnt - 1]) + (MAP_ELEMENT) BOX * 16);
	}

	m_map.set(changes[1], m_map.get(changes[1]) % 16 + (MAP_ELEMENT) MAN * 16);
	m_map.set(changes[0], m_map.get(changes[0]) % 16);
	m_ui_assist.set_man_position(pt[1].y, pt[1].x);
}

void CMainFrame::go_history(INT offset /* 1: forward, -1: go back*/)
{
	HISTORY_ENTRY entry = {0};
	BOOL ret = m_history.get(OUT &entry, offset);
	if (!ret)
		return;
	
	INT step;
	if (offset < 0)
		step = - entry.step;
	else
		step = entry.step;
	
	POINT* pt_man = m_ui_assist.get_man_position();
	INT man_offset = m_map.grid_to_index(pt_man->y, pt_man->x);
	
	INT affected[3] = {0};
	affected[0] = man_offset + step;
	affected[1] = man_offset;
	if (offset < 0)
		affected[2] = man_offset - step * (entry.affected_amount - 2);
	else
		affected[2] = man_offset + step * (entry.affected_amount - 1);
	
	if (offset == -1)
	{
		m_map.set(affected[0], m_map.get(affected[0]) + (MAP_ELEMENT) MAN * 16);
		m_ui_assist.set_man_position(affected[0]);
		if (entry.affected_amount == 2)
			m_map.set(affected[1], m_map.get(affected[1]) % 16);
		else
			m_map.set(affected[1], m_map.get(affected[1]) % 16 + (MAP_ELEMENT) BOX * 16);

		if (entry.affected_amount > 2)
		{
			m_map.set(affected[2], m_map.get(affected[2]) % 16);
			m_view.draw_grid(affected[2], TRUE);
		}
	}
	else
	{
		m_map.set(affected[0], m_map.get(affected[0]) % 16 + (MAP_ELEMENT) MAN * 16);
		m_ui_assist.set_man_position(affected[0]);
		m_map.set(affected[1], m_map.get(affected[1]) % 16);
		
		if (entry.affected_amount > 2)
		{
			m_map.set(affected[2], m_map.get(affected[2]) % 16 + (MAP_ELEMENT) BOX * 16);
			m_view.draw_grid(affected[2], TRUE);
		}
		
	}

	m_view.draw_grid(affected[1], TRUE);
	m_view.draw_grid(affected[0], TRUE);
}

void CMainFrame::set_statusbar_text(INT ids)
{
	TCHAR text[128] = {0};
	::LoadString(_Module.GetResourceInstance(), ids, text, 128);
	set_statusbar_text(text);
}

void CMainFrame::set_statusbar_text(LPTSTR str)
{
	::SendMessage(m_hWndStatusBar, WM_SETTEXT, 0, (LPARAM) str);
}

void CMainFrame::set_tb_game_ui()
{
	if (_g_game_mode == GAME_MODE(PLAY_GAME_MODE))
	{
		m_history_opened = FALSE;
		m_play_satus = FALSE;
	}
	::SendMessage(m_h_tb_game, TB_ENABLEBUTTON, ID_GAME_SAVE, FALSE);
	::SendMessage(m_h_tb_game, TB_ENABLEBUTTON, ID_GAME_PLAY, m_history_opened);
	::SendMessage(m_h_tb_game, TB_ENABLEBUTTON, ID_GAME_STOP, m_history_opened && m_play_satus);

	int index = 4;
	if (m_play_satus) index = 6;
	::SendMessage(m_h_tb_game, TB_CHANGEBITMAP, ID_GAME_PLAY, index);
	RECT rc = {0};
	::SendMessage(m_h_tb_game, TB_GETITEMRECT, ID_GAME_PLAY, (LPARAM) &rc);
	::InvalidateRect(m_h_tb_game, &rc, TRUE);
	
}

void CMainFrame::refresh_ui()
{
	m_pass_finished = FALSE;
	m_ui_assist.refresh();
	m_ui_assist.reset_man_position();
	m_view.set_tips();

	m_view.refresh_map_area();
}


LRESULT CMainFrame::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	if (wParam != PLAY_TIMER_ID)
		return S_OK;
	if (m_play_satus)
		play_history();	
	
	return S_OK;
}

void CMainFrame::reset_opened_history()
{
	m_history.set_cursor_to_start();
	m_map.load(_g_pass_no);
	m_play_satus = FALSE;
	set_tb_game_ui();
	refresh_ui();
}

LRESULT CMainFrame::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (LPNMHDR(lParam)->code != TTN_GETDISPINFO) 
		return S_OK;
	
	LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam; 
    lpttt->hinst = _Module.GetResourceInstance(); 
	lpttt->lpszText = MAKEINTRESOURCE(get_ids_from_ctrl(lpttt->hdr.idFrom));
	bHandled = TRUE;

	return S_OK;
}

void CMainFrame::handle_pass_finished()
{
	TCHAR info[256] = {0};
	INT s_id = IDS_GAME_FINISHED;
	if (_g_pass_no >= _g_pass_amount - 1)
		s_id = IDS_GAME_FINISHED_ALL;
	::LoadString(_Module.GetResourceInstance(), s_id, info, 256);
	INT len = lstrlen(info);
	info[len] ='\n';
	info[len + 1] ='\n';
	m_info_dlg.set_info(info);
	show_info_dlg(TRUE);
	m_pass_finished = TRUE;
}


void CMainFrame::play_next()
{
	if (_g_pass_no < _g_pass_amount - 1)
	{
		_g_pass_no ++;
		if (_g_pass_no > _g_player_passed_amount)
		{
			_g_player_passed_amount = _g_pass_no;
			write_player_pass_no();
		}
		
		load_data_and_start();
	}
}

void CMainFrame::load_data_and_start()
{
	m_map.load(_g_pass_no);
	m_history.init(FALSE);
	
	refresh_ui();
	set_tb_game_ui();
	ShowToolBar();	
	if (m_p_explorer != NULL) {
		delete m_p_explorer;
		m_p_explorer = NULL;
	}

	m_p_explorer = new CPathAI(&m_map);
}


void CMainFrame::show_info_dlg(BOOL showit)
{
	m_info_dlg.CenterWindow(m_hWnd);
	if (showit)
		m_info_dlg.ShowWindow(SW_SHOW);
	else
		m_info_dlg.ShowWindow(SW_HIDE);
}

INT CMainFrame::get_player_pass_no(LPTSTR player)
{
	TCHAR full_path[256] = {0};
	get_full_path(full_path, 256, PLAYER_PASS_FILE);
	CFileM file(full_path, OPEN_EXISTING);
	if (INVALID_HANDLE_VALUE == file.m_h_file)
		return 0;
	
	INT pass_no = 0;
	file.set_file_pointer(0);
	file.read(&pass_no, sizeof(INT));

	return pass_no;
}

BOOL CMainFrame::write_player_pass_no(LPTSTR player)
{
	TCHAR full_path[256] = {0};
	get_full_path(full_path, 256, PLAYER_PASS_FILE);
	CFileM file(full_path, OPEN_ALWAYS);
	if (INVALID_HANDLE_VALUE == file.m_h_file)
		return FALSE;
	
	file.set_file_pointer(0);
	file.write(&_g_player_passed_amount, sizeof(INT));

	return TRUE;
}

BOOL CMainFrame::start_game()
{
	_g_game_mode = (GAME_MODE) PLAY_GAME_MODE;
	if (_g_pass_amount < 1)
	{
		show_no_map_info();	
		return FALSE;
	}
	_g_pass_no = get_player_pass_no(NULL);
	load_data_and_start();
	return TRUE;
}

void CMainFrame::show_no_map_info()
{
	TCHAR info[256] = {0};
	::LoadString(_Module.GetResourceInstance(), IDS_NO_MAP, info, 256);
	m_info_dlg.set_info(info);
	show_info_dlg(TRUE);
}

LRESULT CMainFrame::OnForceSet(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	INT force_id[4] = {ID_FORCE_LEVEL_1, ID_FORCE_LEVEL_2, ID_FORCE_LEVEL_3, ID_FORCE_NO_LIMIT};
	INT force[4]	= {1, 2, 3, 999};
	HMENU hmenu = m_CmdBar.m_hMenu;
	for (INT i = 0; i < 4; i ++)
	{
		if (force_id[i] == wID)
		{
			CheckMenuItem(hmenu, force_id[i], MF_CHECKED);
			m_man_force_level = force[i];
		}
		else
			CheckMenuItem(hmenu, force_id[i], MF_UNCHECKED);
		
	}
	

	return S_OK;
}

LRESULT CMainFrame::import_maps(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CMapExImporter importer;
	importer.do_import();

	return S_OK;
}