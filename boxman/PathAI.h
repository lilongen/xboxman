// IntelligentPath.h: interface for the CPathAI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHAI_H__2AD208EB_4A04_4F32_89F1_9AC6B2024E8D__INCLUDED_)
#define AFX_PATHAI_H__2AD208EB_4A04_4F32_89F1_9AC6B2024E8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include <map>
#include <vector>
#include "Map.h"
using namespace std;

class CPathAI  
{
public:
	struct Entry{
		INT right;
		INT rights;
		INT lasts[4];
	};

public:
	CPathAI(CMap* pMap);
	virtual ~CPathAI();
	void setDestination(int dest);
	void setDestination(POINT& dest);
	POINT& getDestination();
	void setOrigin(int origin);
	void setOrigin(POINT& origin);
	POINT& getOrigin();
	std::vector <int>& getPaths();
	std::vector <int>& getPaths(POINT& D);
	std::vector <int>& getPaths(POINT& O, POINT& D);
	void constructPath();

private:
	void initEntrys();
	void index2Point(INT index, IN OUT POINT& pt);
	INT point2Index(POINT& pt);

private:
	CMap* pMap;
	Entry* entrys;
	INT COLS;
	INT ROWS;
	POINT destination;
	POINT origin;
	std::vector <int> paths;

	std::set <INT> s;
	std::set <INT>::iterator si;
};

#endif // !defined(AFX_PATHAI_H__2AD208EB_4A04_4F32_89F1_9AC6B2024E8D__INCLUDED_)
