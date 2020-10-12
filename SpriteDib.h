#pragma once
#include "framework.h"
#include "month4_Project.h"

class CSpriteDib {

public:

	//DIB 스프라이ㄷ트 구조체

	typedef struct st_SPRITE {
		BYTE *bypImage;
		int iWidth;
		int iHeight;
		int iPitch;

		int iCenterPointX;
		int iCenterPointY;
	}st_SPRITE;

	CSpriteDib(int iMaxSprite, DWORD dwColorKey);
	virtual ~CSpriteDib();

	BOOL LoadDibSprite(int iSpriteIndex, WCHAR* szFileName, int iCenterPointX, int iCenterPointY);

	void ReleaseSprite(int iSpriteIndex);

	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100, BOOL bPlayer = FALSE);

	void DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

protected:
	
	int			_iMaxSprite;
	st_SPRITE*	_stpSprite;
	
	DWORD		_dwColorKey;
};