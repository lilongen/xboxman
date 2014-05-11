// FileM.h: interface for the CFileM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEM_H__40807910_15A0_4CE3_8C1B_C32C0468672B__INCLUDED_)
#define AFX_FILEM_H__40807910_15A0_4CE3_8C1B_C32C0468672B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileM  
{
public:
	CFileM(LPTSTR file, DWORD dwCreationDisposition = OPEN_ALWAYS);
	virtual ~CFileM();
	void write(LPVOID lp_data, INT data_len);
	void read(LPVOID lp_data, INT data_len);
	void set_file_pointer(INT pos, INT start = FILE_BEGIN);
	void close();

public:
	HANDLE m_h_file;
};

#endif // !defined(AFX_FILEM_H__40807910_15A0_4CE3_8C1B_C32C0468672B__INCLUDED_)
