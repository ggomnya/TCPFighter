#pragma once
#include "Framework.h"
#include "month4_Project.h"

class CFrameSkip {
private:
	DWORD _dwSystemTick;
	int _iMaxFPS;
	int _iOneFrameTick;
	int _iTick;
public:
	CFrameSkip(DWORD dwSystemTick, int iMaxFPS, int iOneFrameTick, int iTick);
	~CFrameSkip();
	BOOL FrameSkip();
};