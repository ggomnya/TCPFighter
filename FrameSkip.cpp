#include "FrameSkip.h"

CFrameSkip::CFrameSkip(DWORD dwSystemTick, int iMaxFPS, int iOneFrameTick, int iTick) {
	_dwSystemTick = dwSystemTick;
	_iMaxFPS = iMaxFPS;
	_iOneFrameTick = iOneFrameTick;
	_iTick = iTick;
}
CFrameSkip::~CFrameSkip() {
}

BOOL CFrameSkip::FrameSkip() {
	DWORD CurTime = timeGetTime();
	_iOneFrameTick = CurTime - _dwSystemTick;
	_iTick = _iMaxFPS - _iOneFrameTick;
	if(_iTick >=0) {
		Sleep(_iTick);
		_dwSystemTick = CurTime + _iTick;
		return TRUE;
	}
	else if(_iTick >-20){
		_dwSystemTick = CurTime + _iTick;
		return TRUE;
	}
	else {
		_dwSystemTick = CurTime + _iTick;
		return FALSE;
	}
}