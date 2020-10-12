#pragma once
#include "framework.h"
#include "month4_Project.h"
#include "Enum.h"

class CBaseObject {
protected:
	BOOL	_bEndFrame;
	DWORD	_dwActionInput;
	int		_iCurX;
	int		_iCurY;
	int		_iDelayCount;
	int		_iFrameDelay;
	int		_iObjectID;
	int		_iObjectType;
	int		_iSpriteEnd;
	int		_iSpriteNow;
	int		_iSpriteStart;

public:

	CBaseObject(int iObjectID, int iObjectType);
	~CBaseObject();

	void ActionInput(DWORD dwAction);

	int GetCurX();
	int GetCurY();
	int GetObjectID();
	int GetObjectType();
	int GetSprite();
	int GetDelayCount();

	BOOL IsEndFrame();
	void NextFrame();

	virtual BOOL Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch) = 0;
	virtual BOOL Run(DWORD dwParam) = 0;
	void SetPosition(int iCurX, int iCurY);
	void SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay);
};

class CPlayerObject : public CBaseObject {
private:

	BOOL	_bPlayerCharacter;
	char	_chHP;
	DWORD	_dwActionCur;
	DWORD	_dwActionOld;
	int		_iDirCur;
	int		_iDirOld;

public:

	CPlayerObject(BOOL bPlayerCharacter, int iObjectID, int iObjectType);
	~CPlayerObject();

	void ActionProc();
	int GetDirection();
	char GetHP();
	DWORD GetCurAction();
	void InputActionProc();
	BOOL IsPlayer();
	BOOL Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);
	BOOL Run(DWORD dwParam);
	
	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();
	void SetActionMove(int iDirCur);
	void SetActionStand();

	void SetDirection(int iDir);
	void SetHP(char chHP);

};

class CEffectObject : public CBaseObject {
private:
	BOOL	_bEffectStart;
	DWORD	_dwAttackID;

public:

	CEffectObject(BOOL bEffectStart, DWORD dwAttackID, int iObjectID, int iObjectType);
	~CEffectObject();
	BOOL Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);
	BOOL Run(DWORD dwParam);
};