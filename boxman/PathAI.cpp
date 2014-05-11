// INTelligentPath.cpp: implementation of the CPathAI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PathAI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPathAI::CPathAI(CMap* pMap) {
	this->pMap = pMap;
	COLS = pMap->m_p_ph->cols;
	ROWS = pMap->m_p_ph->rows;
	origin.x = origin.y = -1;
	destination.x = destination.y = -1;
	
	entrys = new Entry[COLS * ROWS];
}

CPathAI::~CPathAI() {
	if (entrys != NULL) {
		delete[] entrys;
	}

	entrys = NULL;
}

void CPathAI::initEntrys() {
	s.clear();
	paths.clear();
	ZeroMemory(entrys, sizeof(Entry) * COLS * ROWS);

	for (INT y = 0; y < ROWS; y++) {
		for (INT x = 0; x < COLS; x++) {
			INT index = y * COLS  + x;
			Entry* entry = (entrys + index);
			entry->right	= 1;
			entry->rights	= INT_MAX;
			entry->lasts[0] = -1;
			entry->lasts[1] = -1;
			entry->lasts[2] = -1;
			entry->lasts[3] = -1;
			if (pMap->isBarrier(index)) {
				entry->right = INT_MAX;
				entry->rights = INT_MAX;
				s.insert(index);
			}

			if (pMap->isMan(index)) {
				entry->rights = 0;
				index2Point(index, origin);

				s.insert(index);
			}
		}
	}
}

void CPathAI::setDestination(INT dest) {
	index2Point(dest, destination);
}

void CPathAI::setDestination(POINT& dest) {
	destination.x = dest.x;
	destination.y = dest.y;
}

void CPathAI::setOrigin(INT origin) {
	index2Point(origin, this->origin);
}

void CPathAI::setOrigin(POINT& origin) {
	this->origin.x = origin.x;
	this->origin.y = origin.y;
}


std::vector <int>& CPathAI::getPaths() {
	return getPaths(origin, destination);
}

std::vector <int>& CPathAI::getPaths(POINT& D) {
	return getPaths(origin, D);
}

std::vector <int>& CPathAI::getPaths(POINT& O, POINT& D) {
	initEntrys();

	INT offsets[] = {-COLS, 1, COLS, -1};
	
	std::map <int, set <int>, less<int> > operations;
	std::set <int> initOrigins;
	initOrigins.insert(point2Index(origin));
	operations[0] = initOrigins;

	TCHAR szDebug[256] = {0};
	for (INT i = 0; i < INT_MAX ; i++) {
		std::set <INT>& pts =  operations[i];
		std::set <INT> nextStepOrigins;
		
#ifdef _DEBUG		
wsprintf(szDebug, "-------------------------------\nNO. step: %02d\n", i);
OutputDebugString(szDebug);
#endif

		bool runNext = false;
		for (si = pts.begin(); si != pts.end(); si++) {
#ifdef _DEBUG
POINT p = {-1, -1}; 
index2Point(*si, p);
wsprintf(szDebug, "\nO (%02d, %02d)\n", p.x, p.y);
OutputDebugString(szDebug);
#endif
			for (INT j = 0; j < 4; j++) {
				INT index = *si + offsets[j];
				
#ifdef _DEBUG
index2Point(index, p);
wsprintf(szDebug, "%02d -> (%02d, %02d)\n", j, p.x, p.y);
OutputDebugString(szDebug);
#endif
				
				if (index < 0 || index >= COLS * ROWS) {
					continue;
				}

				if (s.find(index) != s.end()) {
					continue;
				}
				
				if (entrys[*si].rights + entrys[index].right < entrys[index].rights) {
					entrys[index].rights = entrys[*si].rights + entrys[index].right;
					entrys[index].lasts[0] = *si;
					
					nextStepOrigins.insert(index);
					runNext = true;
				}
			}
		}

		if (!runNext) {
			break;
		}
		
		for (si = nextStepOrigins.begin(); si != nextStepOrigins.end(); si++) {
			if (*si == point2Index(destination)) {//destination reached
				constructPath();
	
				return paths;
			}

			s.insert(*si);
		}

		operations[i + 1] = nextStepOrigins;
	}

	return paths;
}

void CPathAI::constructPath() {
	INT indexD = point2Index(destination);
	INT indexO = point2Index(origin);
	paths.push_back(indexD);

	Entry* entry = entrys + indexD;
	TCHAR szDebug[256] = {0};

#ifdef _DEBUG
OutputDebugString("Re-path is: \n");
wsprintf(szDebug, "(%02d, %02d)\n", destination.x, destination.y);
OutputDebugString(szDebug);
#endif
	
	while (entry->lasts[0] != indexO) {
#ifdef _DEBUG		
POINT pt = {-1, -1};
index2Point(entry->lasts[0], pt);
wsprintf(szDebug, "(%02d, %02d)\n", pt.x, pt.y);
OutputDebugString(szDebug);
#endif
		
		paths.push_back(entry->lasts[0]);
		entry = entrys + entry->lasts[0];
	}

#ifdef _DEBUG		
wsprintf(szDebug, "(%02d, %02d)\n", origin.x, origin.y);
OutputDebugString(szDebug);
#endif
	paths.push_back(indexO);

	return;
}


void CPathAI::index2Point(INT index, IN OUT POINT& pt) 
{
	pt.x = index % COLS;
	pt.y = index / COLS;
}

INT CPathAI::point2Index(POINT& pt) 
{
	return pt.y * COLS + pt.x;
}

POINT& CPathAI::getDestination() {
	return destination;
}

POINT& CPathAI::getOrigin() {
	return origin;
}

