#if !defined(__PROJECT_H_)
#define __PROJECT_H_ TRUE

#if !defined(__DEBUG__)
//#define __DEBUG__ TRUE
#define DEBUG_FILE _T("debug.log")
#endif

#define RESOURCE_DLL		_T("res.dll")
#define PASS_ARCHIVE		_T("map.dat")
#define PLAYER_PASS_FILE	_T("player.dat")

typedef enum {
	PLAY_GAME_MODE		= 1,
	PLAY_RECORD_MODE	= 2,
	EDIT_MAP_MODE		= 3,
} GAME_MODE;


#endif //!defined(__PROJECT_H_)