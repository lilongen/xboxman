// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F4687357_BAFA_4F38_B4D9_8BA204F35332__INCLUDED_)
#define AFX_STDAFX_H__F4687357_BAFA_4F38_B4D9_8BA204F35332__INCLUDED_

// Change these values to use different versions
#define WINVER		0x0400
//#define _WIN32_WINNT	0x0400
#define _WIN32_IE	0x0400
#define _RICHEDIT_VER	0x0100

#include <atlbase.h>
#include <atlapp.h>
#include "project.h"
extern INT _g_pass_no;
extern INT _g_pass_amount;
extern INT _g_player_passed_amount;
extern GAME_MODE _g_game_mode;
extern CAppModule _Module;


#include <atlwin.h>
#include "../boxman_res/resource.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F4687357_BAFA_4F38_B4D9_8BA204F35332__INCLUDED_)
