#include <Windows.h>
#include "ScreenDib.h"

CScreenDib::CScreenDib(int iWidth, int iHeight, int iColorBit) {
	memset(&_stDibInfo, 0, sizeof(BITMAPINFO));
	_bypBuffer = NULL;

	_iWidth = 0;
	_iHeight = 0;
	_iPitch = 0;
	_iBufferSize = 0;
	_iColorBit = 0;


	CreateDibBuffer(iWidth, iHeight, iColorBit);
}

CScreenDib::~CScreenDib() {
	ReleaseDibBuffer();
}

void CScreenDib::CreateDibBuffer(int iWidth, int iHeight, int iColorBit) {
	_iWidth = iWidth;
	_iHeight = iHeight;
	_iColorBit = iColorBit;

	_iPitch = ((_iWidth * (_iColorBit / 8)) + 3) & ~3;
	_iBufferSize = _iPitch * _iHeight;

	//DibInfo 헤더 생성
	_stDibInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	_stDibInfo.bmiHeader.biWidth = _iWidth;
	_stDibInfo.bmiHeader.biHeight = -_iHeight;
	_stDibInfo.bmiHeader.biPlanes = 1;
	_stDibInfo.bmiHeader.biBitCount = _iColorBit;
	_stDibInfo.bmiHeader.biCompression = 0;
	_stDibInfo.bmiHeader.biSizeImage = _iBufferSize;
	_stDibInfo.bmiHeader.biXPelsPerMeter = 0;
	_stDibInfo.bmiHeader.biYPelsPerMeter = 0;
	_stDibInfo.bmiHeader.biClrUsed = 0;
	_stDibInfo.bmiHeader.biClrImportant = 0;

	//버퍼 생성
	_bypBuffer = new BYTE[_iBufferSize];
	memset(_bypBuffer, 0xff, _iBufferSize);

}

void CScreenDib::ReleaseDibBuffer(void) {
	_iWidth = 0;
	_iHeight = 0;
	_iPitch = 0;
	_iBufferSize = 0;

	memset(&_stDibInfo, 0x00, sizeof(BITMAPINFO));

	if (_bypBuffer != NULL)
		delete[] _bypBuffer;

	_bypBuffer = NULL;
}

void CScreenDib::Flip(HWND hWnd, int iX, int iY) {
	if (_bypBuffer == NULL) return;

	RECT Rect;
	HDC hDC;

	GetWindowRect(hWnd, &Rect);
	hDC = GetDC(hWnd);

	int i = SetDIBitsToDevice(hDC, 0, 0, _iWidth, _iHeight, 0, 0, 0, _iHeight, _bypBuffer, &_stDibInfo, DIB_RGB_COLORS);

	//화면에 프레임 표시 부분.

	
	static char szFrame[5];
	static int iFrame = 0;
	static DWORD dwTick = 0;
	iFrame++;
	if (dwTick + 1000 < timeGetTime()) {
		wsprintf((wchar_t*)szFrame, L"%d", iFrame);
		iFrame = 0;
		dwTick = timeGetTime();
	}
	TextOut(hDC, 0, 0, (wchar_t*)szFrame, (int)wcslen((wchar_t*)szFrame));

	ReleaseDC(hWnd, hDC);

}

BYTE* CScreenDib::GetDibBuffer(void) {
	return _bypBuffer;
}
int CScreenDib::GetWidth(void) {
	return _iWidth;
}
int CScreenDib::GetHeight(void) {
	return _iHeight;
}
int CScreenDib::GetPitch(void) {
	return _iPitch;
}