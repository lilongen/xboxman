// boxman.cpp : main source file for boxman.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include "..\boxman_res\resource.h"
#include "boxmanView.h"
#include "aboutdlg.h"
#include "mainfrm.h"
INT	_g_pass_no;
INT _g_pass_amount;
INT _g_player_passed_amount;
GAME_MODE _g_game_mode;
CAppModule _Module;


HINSTANCE get_resource_handle();
void get_full_path(LPTSTR sz_path, INT length, LPTSTR file_name = 0);

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	_Module.m_hInstResource = get_resource_handle();
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::FreeLibrary(_Module.GetResourceInstance());
	::CoUninitialize();

	return nRet;
}

void get_full_path(LPTSTR sz_path, INT length, LPTSTR file_name)
{
    if (GetModuleFileName( _Module.GetModuleInstance(), sz_path, length))
    {
        char ch = '\\';
        char* p = strrchr( sz_path, ch );
        if (p)
            lstrcpyn(sz_path, sz_path, p - sz_path + 2 );
    }

	if (file_name)
		lstrcat(sz_path, file_name);
}

HINSTANCE get_resource_handle()
{
    TCHAR sz_dll[_MAX_PATH] = {0};

	get_full_path(sz_dll, _MAX_PATH, RESOURCE_DLL);
    
    HINSTANCE h_res = ::LoadLibrary(sz_dll);

	return h_res;
}