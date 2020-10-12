#include "framework.h"
#include "SpriteDib.h"

CSpriteDib::CSpriteDib(int iMaxSprite, DWORD dwColorKey) {
	_iMaxSprite = iMaxSprite;
	_dwColorKey = dwColorKey;

	_stpSprite = NULL;

	_stpSprite = new st_SPRITE[_iMaxSprite];
	memset(_stpSprite, 0, sizeof(st_SPRITE) * _iMaxSprite);
}

CSpriteDib::~CSpriteDib() {
	int iCount;

	for (iCount = 0; iCount < _iMaxSprite; iCount++) {
		ReleaseSprite(iCount);
	}
}

BOOL CSpriteDib::LoadDibSprite(int iSpriteIndex, WCHAR* szFileName, int iCenterPointX, int iCenterPointY) {

	FILE* hFile;
	DWORD dwRead;
	int iPitch;
	int iImageSize;

	BITMAPFILEHEADER stFileHeader;
	BITMAPINFOHEADER stInfoHeader;

	int err = _wfopen_s(&hFile, szFileName, L"rb");
	if (err != 0) {
		return FALSE;
	}

	ReleaseSprite(iSpriteIndex);
	fread_s(&stFileHeader, sizeof(BITMAPFILEHEADER), sizeof(BITMAPFILEHEADER), 1, hFile);
	if (0x4d42 == stFileHeader.bfType) {

		fread_s(&stInfoHeader, sizeof(BITMAPINFOHEADER), sizeof(BITMAPINFOHEADER), 1, hFile);
		if (32 == stInfoHeader.biBitCount) {

			iPitch = (stInfoHeader.biWidth * 4) + 3 & ~3;

			_stpSprite[iSpriteIndex].iWidth = stInfoHeader.biWidth;
			_stpSprite[iSpriteIndex].iHeight = stInfoHeader.biHeight;
			_stpSprite[iSpriteIndex].iPitch = iPitch;

			iImageSize = iPitch * stInfoHeader.biHeight;
			_stpSprite[iSpriteIndex].bypImage = new BYTE[iImageSize];

			BYTE* bypTempBuffer = new BYTE[iImageSize];
			BYTE* bypSpriteTemp = _stpSprite[iSpriteIndex].bypImage;
			BYTE* bypTurnTemp;
			int iCount;

			fread_s(bypTempBuffer, iImageSize, iImageSize, 1, hFile);

			bypTurnTemp = bypTempBuffer + iPitch * (stInfoHeader.biHeight - 1);

			for (iCount = 0; iCount < stInfoHeader.biHeight; iCount++) {
				memcpy(bypSpriteTemp, bypTurnTemp, iPitch);
				bypSpriteTemp += iPitch;
				bypTurnTemp -= iPitch;
			}

			delete[] bypTempBuffer;
			_stpSprite[iSpriteIndex].iCenterPointX = iCenterPointX;
			_stpSprite[iSpriteIndex].iCenterPointY = iCenterPointY;

			fclose(hFile);
			return TRUE;
		}
	}
	fclose(hFile);
	return FALSE;
}

void CSpriteDib::ReleaseSprite(int iSpriteIndex) {
	if (_iMaxSprite <= iSpriteIndex)
		return;
	if (NULL != _stpSprite[iSpriteIndex].bypImage) {

		delete[] _stpSprite[iSpriteIndex].bypImage;
		memset(&_stpSprite[iSpriteIndex], 0, sizeof(st_SPRITE));
	}
}

void CSpriteDib::DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
	int iDestHeight, int iDestPitch, int iDrawLen, BOOL bPlayer) {

	if (iSpriteIndex >= _iMaxSprite)
		return;
	if (_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = (st_SPRITE*)&_stpSprite[iSpriteIndex];

	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountX, iCountY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);
	
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//클리핑

	//상
	if (iDrawY < 0) {
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));
		iDrawY = 0;
	}

	//하
	if (iDestHeight <= iDrawY + stpSprite->iHeight) {
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	//좌
	if (iDrawX < 0) {
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		iDrawX = 0;
	}

	//우
	if (iDestWidth <= iDrawX + stpSprite->iWidth) {
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);
	}

	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
		return;

	//화면 찍을 위치 이동
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;
	
	//플레이어는 붉게 표시
	if (bPlayer) {
		for (iCountY = 0; iSpriteHeight > iCountY; iCountY++) {
			for (iCountX = 0; iSpriteWidth > iCountX; iCountX++) {
				if (_dwColorKey != (*dwpSprite & 0x00ffffff)) {
					*dwpDest = (*dwpSprite) & 0x00ff9999;
				}
				dwpDest++;
				dwpSprite++;
			}

			bypDestOrigin = bypDestOrigin + iDestPitch;
			bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

			dwpDest = (DWORD*)bypDestOrigin;
			dwpSprite = (DWORD*)bypSpriteOrigin;
		}
	}
	else {
		for (iCountY = 0; iSpriteHeight > iCountY; iCountY++) {
			for (iCountX = 0; iSpriteWidth > iCountX; iCountX++) {
				if (_dwColorKey != (*dwpSprite & 0x00ffffff)) {
					*dwpDest = *dwpSprite;
				}
				dwpDest++;
				dwpSprite++;
			}

			bypDestOrigin = bypDestOrigin + iDestPitch;
			bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

			dwpDest = (DWORD*)bypDestOrigin;
			dwpSprite = (DWORD*)bypSpriteOrigin;
		}
	}

}

void CSpriteDib::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
	int iDestHeight, int iDestPitch, int iDrawLen) {

	if (iSpriteIndex >= _iMaxSprite)
		return;
	if (_stpSprite[iSpriteIndex].bypImage == NULL)
		return;

	st_SPRITE* stpSprite = (st_SPRITE*)&_stpSprite[iSpriteIndex];

	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountX, iCountY;


	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);
	DWORD* dwpDest = (DWORD*)bypDest;
	int curDrawX = iDrawX;
	int curDrawY = iDrawY;
	//반복하면서 타일 찍기
	for (int i = 1; i <=9; i++) {
		for (int j = 1; j <=11; j++) {
			iSpriteWidth = stpSprite->iWidth;
			iSpriteHeight = stpSprite->iHeight;
			dwpSprite = (DWORD*)(stpSprite->bypImage);
			//클리핑
			//상
			if (curDrawY < 0) {
				iSpriteHeight = iSpriteHeight - (-curDrawY);
				dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-curDrawY));
				curDrawY = 0;
			}
			//하
			if (iDestHeight <= curDrawY + stpSprite->iHeight) {
				iSpriteHeight -= ((curDrawY + stpSprite->iHeight) - iDestHeight);
			}
			//좌
			if (curDrawX < 0) {
				iSpriteWidth = iSpriteWidth - (-curDrawX);
				dwpSprite = dwpSprite + (-curDrawX);
				curDrawX = 0;
			}
			//우
			if (iDestWidth <= curDrawX + stpSprite->iWidth) {
				iSpriteWidth -= ((curDrawX + stpSprite->iWidth) - iDestWidth);
			}
			if (iSpriteHeight <= 0 || iSpriteWidth <= 0) {
				break;
			}
			//화면 찍을 위치 이동
			dwpDest = (DWORD*)(((BYTE*)(bypDest + curDrawX*4) + (curDrawY * iDestPitch)));

			BYTE* bypDestOrigin = (BYTE*)dwpDest;
			BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

			for (iCountY = 0; iSpriteHeight > iCountY; iCountY++) {
				for (iCountX = 0; iSpriteWidth > iCountX; iCountX++) {
					*dwpDest = *dwpSprite;

					dwpDest++;
					dwpSprite++;
				}

				bypDestOrigin = bypDestOrigin + iDestPitch;
				bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

				dwpDest = (DWORD*)bypDestOrigin;
				dwpSprite = (DWORD*)bypSpriteOrigin;
			}
			curDrawX = iDrawX + j*64;
			curDrawY = iDrawY + (i-1)*64;
		}
		curDrawX = iDrawX;
		curDrawY = iDrawY + i * 64;
	}
}