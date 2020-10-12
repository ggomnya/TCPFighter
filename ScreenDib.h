#pragma once
#include "framework.h"
#include "month4_Project.h"

class CScreenDib {
public:
	
	CScreenDib(int iWidth, int iHeight, int iColorBit);
	virtual ~CScreenDib();

protected:
	
	void CreateDibBuffer(int iWidth, int iHeight, int iColorBit);
	void ReleaseDibBuffer(void);

public:

	void Flip(HWND hWnd, int iX = 0, int iY = 0);

	BYTE* GetDibBuffer(void);
	int GetWidth(void);
	int GetHeight(void);
	int GetPitch(void);

protected:

	BITMAPINFO	_stDibInfo;
	BYTE*		_bypBuffer;

	int			_iWidth;
	int			_iHeight;
	int			_iPitch;
	int			_iColorBit;
	int			_iBufferSize;

};