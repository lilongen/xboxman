// FileM.cpp: implementation of the CFileM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileM.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void get_full_path(LPTSTR sz_path, INT length, LPTSTR file_name = 0);

CFileM::CFileM(LPTSTR file, DWORD dwCreationDisposition)
{
    m_h_file = ::CreateFile(file, 
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ,
                                 NULL,
                                 dwCreationDisposition,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL );
}

void CFileM::write(LPVOID lp_data, INT data_len)
{
	DWORD dw_offset = 0;
	
    BOOL ret = ::WriteFile(m_h_file,
                 lp_data,
                 data_len,
                 &dw_offset,
                 NULL);
	
	if (ret == 0)
		DWORD err = ::GetLastError();
}

void CFileM::read(LPVOID lp_data, INT data_len)
{
	DWORD dw = 0;
	::ReadFile(m_h_file, lp_data, data_len, &dw, NULL);
}

void CFileM::set_file_pointer(INT pos, INT start)
{
	::SetFilePointer(m_h_file, pos, NULL, start);
}

CFileM::~CFileM()
{
	close();
}

void CFileM::close()
{
	if (INVALID_HANDLE_VALUE != m_h_file)
		::CloseHandle(m_h_file);
}
