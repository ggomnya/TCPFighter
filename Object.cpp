#include "Object.h"
#include "SpriteDib.h"
#include "TCPNetwork.h"



extern CSpriteDib g_cSprite;
extern int g_CameraX;
extern int g_CameraY;

//CBaseObject
CBaseObject::CBaseObject(int iObjectID, int iObjectType) {
	_iObjectID = iObjectID;
	_iObjectType = iObjectType;
}
CBaseObject::~CBaseObject() {

}

void CBaseObject::ActionInput(DWORD dwAction) {
	_dwActionInput = dwAction;
}

int CBaseObject::GetCurX() {
	return _iCurX;
}
int CBaseObject::GetCurY() {
	return _iCurY;
}
int CBaseObject::GetObjectID() {
	return _iObjectID;
}
int CBaseObject::GetObjectType() {
	return _iObjectType;
}
int CBaseObject::GetSprite() {
	return _iSpriteNow;
}

int CBaseObject::GetDelayCount() {
	return _iDelayCount;
}

BOOL CBaseObject::IsEndFrame() {
	return _bEndFrame;
}
void CBaseObject::NextFrame() {
	if (_iSpriteStart < 0)
		return;

	_iDelayCount++;
	if (_iDelayCount >= _iFrameDelay) {
		_iDelayCount = 0;

		_iSpriteNow++;

		if (_iSpriteNow > _iSpriteEnd) {
			_iSpriteNow = _iSpriteStart;
			_bEndFrame = TRUE;
		}
	}
}

void CBaseObject::SetPosition(int iCurX, int iCurY) {
	_iCurX = iCurX;
	_iCurY = iCurY;
}
void CBaseObject::SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay) {
	_iSpriteStart = iSpriteStart;
	_iSpriteEnd = iSpriteMax;
	_iFrameDelay = iFrameDelay;

	_iSpriteNow = iSpriteStart;
	_iDelayCount = 0;
	_bEndFrame = FALSE;
}


//CPlayerObject

CPlayerObject::CPlayerObject(BOOL bPlayerCharacter, int iObjectID, int iObjectType) : CBaseObject(iObjectID, iObjectType) {
	_bPlayerCharacter = bPlayerCharacter;
	_dwActionCur = dfACTION_STAND;
	_dwActionOld = dfACTION_STAND;
	_dwActionInput = dfACTION_STAND;
}
CPlayerObject::~CPlayerObject() {

}

void CPlayerObject::ActionProc() {

	if (_bPlayerCharacter) {
		switch (_dwActionCur) {
		case dfACTION_ATTACK1:

		case dfACTION_ATTACK2:
		case dfACTION_ATTACK3:
			if (IsEndFrame()) {
				_dwActionInput = dfACTION_STAND;
				SetActionStand();

			}
			break;
		default:
			InputActionProc();
			break;

		}
	}
	//나중에 처리하기
	else {
		switch (_dwActionCur) {
		case dfACTION_ATTACK1:

		case dfACTION_ATTACK2:
		case dfACTION_ATTACK3:
			if (_dwActionInput >= dfACTION_ATTACK1 && _dwActionInput <= dfACTION_ATTACK3) {
				InputActionProc();
			}

			if (IsEndFrame()) {
				_dwActionInput = dfACTION_STAND;
				SetActionStand();
			}
			
			break;
		default:
			InputActionProc();
			break;

		}
	}
	

}
int CPlayerObject::GetDirection() {
	return _iDirCur;
}
char CPlayerObject::GetHP() {
	return _chHP;
}

DWORD CPlayerObject::GetCurAction() {
	return _dwActionCur;
}
void CPlayerObject::InputActionProc() {

	switch (_dwActionInput) {
	case dfACTION_MOVE_LL:
		SetActionMove(LEFT);
		if (GetCurX() - 3 > dfRANGE_MOVE_LEFT)
			SetPosition(GetCurX() - 3, GetCurY());
		break;
	case dfACTION_MOVE_LU:
		SetActionMove(LEFT);
		if (GetCurX() - 3 > dfRANGE_MOVE_LEFT)
			SetPosition(GetCurX() - 3, GetCurY());

		if (GetCurY() - 2 > dfRANGE_MOVE_TOP)
			SetPosition(GetCurX(), GetCurY() - 2);

		break;
	case dfACTION_MOVE_UU:
		SetActionMove(_iDirCur);
		if (GetCurY() - 2 > dfRANGE_MOVE_TOP)
			SetPosition(GetCurX(), GetCurY() - 2);

		break;
	case dfACTION_MOVE_RU:
		SetActionMove(RIGHT);
		if (GetCurX() + 3 < dfRANGE_MOVE_RIGHT)
			SetPosition(GetCurX() + 3, GetCurY());

		if (GetCurY() - 2 > dfRANGE_MOVE_TOP)
			SetPosition(GetCurX(), GetCurY() - 2);
		break;
	case dfACTION_MOVE_RR:
		SetActionMove(RIGHT);
		if (GetCurX() + 3 < dfRANGE_MOVE_RIGHT)
			SetPosition(GetCurX() + 3, GetCurY());
		break;
	case dfACTION_MOVE_RD:
		SetActionMove(RIGHT);
		if (GetCurX() + 3 < dfRANGE_MOVE_RIGHT)
			SetPosition(GetCurX() + 3, GetCurY());

		if (GetCurY() + 2 < dfRANGE_MOVE_BOTTOM)
			SetPosition(GetCurX(), GetCurY() + 2);
		break;
	case dfACTION_MOVE_DD:
		SetActionMove(_iDirCur);
		if (GetCurY() + 2 < dfRANGE_MOVE_BOTTOM)
			SetPosition(GetCurX(), GetCurY() + 2);
		break;
	case dfACTION_MOVE_LD:
		SetActionMove(LEFT);
		if (GetCurX() - 3 > dfRANGE_MOVE_LEFT)
			SetPosition(GetCurX() - 3, GetCurY());

		if (GetCurY() + 2 < dfRANGE_MOVE_BOTTOM)
			SetPosition(GetCurX(), GetCurY() + 2);
		break;
	case dfACTION_ATTACK1:
		SetActionAttack1();
		break;
	case dfACTION_ATTACK2:
		SetActionAttack2();
		break;
	case dfACTION_ATTACK3:
		SetActionAttack3();
		break;
	case dfACTION_STAND:
		SetActionStand();
		break;
	}
}
BOOL CPlayerObject::IsPlayer() {
	return _bPlayerCharacter;
}
BOOL CPlayerObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch) {
	
	g_cSprite.DrawSprite(eSHADOW, _iCurX - g_CameraX, _iCurY - g_CameraY, bypDest, iDestWidth, iDestHeight, iDestPitch);

	g_cSprite.DrawSprite(GetSprite(), _iCurX - g_CameraX, _iCurY - g_CameraY, bypDest, iDestWidth, iDestHeight, iDestPitch, 100, _bPlayerCharacter);

	g_cSprite.DrawSprite(eGUAGE_HP, _iCurX - 35 - g_CameraX, _iCurY + 9 - g_CameraY, bypDest, iDestWidth, iDestHeight, iDestPitch, GetHP());

	return TRUE;

}
BOOL CPlayerObject::Run(DWORD dwParam) {
	NextFrame();
	ActionProc();
	return FALSE;
}

void CPlayerObject::SetActionAttack1() {
	if (_bPlayerCharacter) {
		//직렬화 버퍼 적용
		//움직이던 도중
		if (_dwActionCur >= dfACTION_MOVE_LL && _dwActionCur <= dfACTION_MOVE_LD) {
			CPacket cWritePacket(dfMAX_PACKET_BUFFER_SIZE);
			MakePacketCSMoveStop(&cWritePacket, _iDirCur, _iCurX, _iCurY);
			SendPacket(&cWritePacket, dfPACKET_CS_MOVE_STOP);
		}
		/*
		if (_dwActionCur >= dfACTION_MOVE_LL && _dwActionCur <= dfACTION_MOVE_LD) {
			st_NETWORK_PACKET_HEADER Header;
			stPACKET_CS_MOVE_STOP Packet;
			MakePacketCSMoveStop(&Header, &Packet, _iDirCur, _iCurX, _iCurY);
			SendPacket(Header, (char*)&Packet);
		}
		*/
		_dwActionOld = _dwActionCur;
		_dwActionCur = dfACTION_ATTACK1;
		//서버에 보내기
		/*
		st_NETWORK_PACKET_HEADER Header;
		stPACKET_CS_ATTACK1 Packet;
		MakePacketCSAttack1(&Header, &Packet, _iDirCur, _iCurX, _iCurY);
		SendPacket(Header, (char*)&Packet);
		*/

		//직렬화 버퍼 적용
		CPacket cWritePacket(dfMAX_PACKET_BUFFER_SIZE);
		MakePacketCSAttack1(&cWritePacket, _iDirCur, _iCurX, _iCurY);
		SendPacket(&cWritePacket, dfPACKET_CS_ATTACK1);
		if (_iDirCur == LEFT) {
			SetSprite(ePLAYER_ATTACK1_L01, ePLAYER_ATTACK1_L_MAX, dfDELAY_ATTACK1);
		}
		else {
			SetSprite(ePLAYER_ATTACK1_R01, ePLAYER_ATTACK1_R_MAX, dfDELAY_ATTACK1);
		}
	
	}
	else {
		_dwActionOld = _dwActionCur;
		_dwActionCur = dfACTION_ATTACK1;
		if (_iDirCur == LEFT) {
			SetSprite(ePLAYER_ATTACK1_L01, ePLAYER_ATTACK1_L_MAX, dfDELAY_ATTACK1);
		}
		else {
			SetSprite(ePLAYER_ATTACK1_R01, ePLAYER_ATTACK1_R_MAX, dfDELAY_ATTACK1);
		}
		_dwActionInput = dfACTION_STAND;
	}
}
void CPlayerObject::SetActionAttack2() {
	if (_bPlayerCharacter) {
		/*
		if (_dwActionCur >= dfACTION_MOVE_LL && _dwActionCur <= dfACTION_MOVE_LD) {
			st_NETWORK_PACKET_HEADER Header;
			stPACKET_CS_MOVE_STOP Packet;
			MakePacketCSMoveStop(&Header, &Packet, _iDirCur, _iCurX, _iCurY);
			SendPacket(Header, (char*)&Packet);
		}
		*/

		//직렬화 버퍼 적용
		if (_dwActionCur >= dfACTION_MOVE_LL && _dwActionCur <= dfACTION_MOVE_LD) {
			CPacket cWritePacket(dfMAX_PACKET_BUFFER_SIZE);
			MakePacketCSMoveStop(&cWritePacket, _iDirCur, _iCurX, _iCurY);
			SendPacket(&cWritePacket, dfPACKET_CS_MOVE_STOP);
		}
		_dwActionOld = _dwActionCur;
		_dwActionCur = dfACTION_ATTACK2;
		//서버에 보내기
		/*
		st_NETWORK_PACKET_HEADER Header;
		stPACKET_CS_ATTACK2 Packet;
		MakePacketCSAttack2(&Header, &Packet, _iDirCur, _iCurX, _iCurY);
		SendPacket(Header, (char*)&Packet);
		*/

		//직렬화 버퍼 적용
		CPacket cWritePacket(dfMAX_PACKET_BUFFER_SIZE);
		MakePacketCSAttack2(&cWritePacket, _iDirCur, _iCurX, _iCurY);
		SendPacket(&cWritePacket, dfPACKET_CS_ATTACK2);

		if (_iDirCur == LEFT) {
			SetSprite(ePLAYER_ATTACK2_L01, ePLAYER_ATTACK2_L_MAX, dfDELAY_ATTACK2);
		}
		else {
			SetSprite(ePLAYER_ATTACK2_R01, ePLAYER_ATTACK2_R_MAX, dfDELAY_ATTACK2);
		}

	}
	else {
		_dwActionOld = _dwActionCur;
		_dwActionCur = dfACTION_ATTACK2;
		if (_iDirCur == LEFT) {
			SetSprite(ePLAYER_ATTACK2_L01, ePLAYER_ATTACK2_L_MAX, dfDELAY_ATTACK2);
		}
		else {
			SetSprite(ePLAYER_ATTACK2_R01, ePLAYER_ATTACK2_R_MAX, dfDELAY_ATTACK2);
		}
		_dwActionInput = dfACTION_STAND;
	}
}
void CPlayerObject::SetActionAttack3() {
	if (_bPlayerCharacter) {
		/*
		if (_dwActionCur >= dfACTION_MOVE_LL && _dwActionCur <= dfACTION_MOVE_LD) {
			st_NETWORK_PACKET_HEADER Header;
			stPACKET_CS_MOVE_STOP Packet;
			MakePacketCSMoveStop(&Header, &Packet, _iDirCur, _iCurX, _iCurY);
			SendPacket(Header, (char*)&Packet);
		}
		*/

		//직렬화 버퍼 적용
		if (_dwActionCur >= dfACTION_MOVE_LL && _dwActionCur <= dfACTION_MOVE_LD) {
			CPacket cWritePacket(dfMAX_PACKET_BUFFER_SIZE);
			MakePacketCSMoveStop(&cWritePacket, _iDirCur, _iCurX, _iCurY);
			SendPacket(&cWritePacket, dfPACKET_CS_MOVE_STOP);
		}

		_dwActionOld = _dwActionCur;
		_dwActionCur = dfACTION_ATTACK3;
		//서버에 보내기
		/*
		st_NETWORK_PACKET_HEADER Header;
		stPACKET_CS_ATTACK3 Packet;
		MakePacketCSAttack3(&Header, &Packet, _iDirCur, _iCurX, _iCurY);
		SendPacket(Header, (char*)&Packet);
		*/

		//직렬화 버퍼 적용
		CPacket cWritePacket(dfMAX_PACKET_BUFFER_SIZE);
		MakePacketCSAttack3(&cWritePacket, _iDirCur, _iCurX, _iCurY);
		SendPacket(&cWritePacket, dfPACKET_CS_ATTACK3);

		if (_iDirCur == LEFT) {
			SetSprite(ePLAYER_ATTACK3_L01, ePLAYER_ATTACK3_L_MAX, dfDELAY_ATTACK3);
		}
		else {
			SetSprite(ePLAYER_ATTACK3_R01, ePLAYER_ATTACK3_R_MAX, dfDELAY_ATTACK3);
		}

	}
	else {
		_dwActionOld = _dwActionCur;
		_dwActionCur = dfACTION_ATTACK3;
		if (_iDirCur == LEFT) {
			SetSprite(ePLAYER_ATTACK3_L01, ePLAYER_ATTACK3_L_MAX, dfDELAY_ATTACK3);
		}
		else {
			SetSprite(ePLAYER_ATTACK3_R01, ePLAYER_ATTACK3_R_MAX, dfDELAY_ATTACK3);
		}
		_dwActionInput = dfACTION_STAND;
	}
}
void CPlayerObject::SetActionMove(int iDirCur) {
	_iDirOld = _iDirCur;
	_iDirCur = iDirCur;
	//움직이지 않았을 때
	if (_dwActionCur < dfACTION_MOVE_LL || _dwActionCur > dfACTION_MOVE_LD) {
		_dwActionOld = _dwActionCur;
		_dwActionCur = _dwActionInput;
		//서버에 통보하기 
		/*
		if (_bPlayerCharacter) {
			st_NETWORK_PACKET_HEADER Header;
			stPACKET_CS_MOVE_START Packet;
			MakePacketCSMoveStart(&Header, &Packet, _dwActionCur, _iCurX, _iCurY);
			SendPacket(Header, (char*)&Packet);
		}
		*/
		//직렬화 버퍼 적용
		if (_bPlayerCharacter) {
			CPacket cWritePacket(dfMAX_PACKET_BUFFER_SIZE);
			MakePacketCSMoveStart(&cWritePacket, _dwActionCur, _iCurX, _iCurY);
			SendPacket(&cWritePacket, dfPACKET_CS_MOVE_START);
		}

		if (_iDirCur == LEFT) {
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		}
		else {
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		}
	}
	//움직일 때
	else {
		//서버에 통보하기
		if (_dwActionCur != _dwActionInput) {
			_dwActionOld = _dwActionCur;
			_dwActionCur = _dwActionInput;
			/*
			if (_bPlayerCharacter) {
				st_NETWORK_PACKET_HEADER Header;
				stPACKET_CS_MOVE_START Packet;
				MakePacketCSMoveStart(&Header, &Packet, _dwActionCur, _iCurX, _iCurY);
				SendPacket(Header, (char*)&Packet);
			}
			*/
			//직렬화 버퍼 적용
			if (_bPlayerCharacter) {
				CPacket cWritePacket(dfMAX_PACKET_BUFFER_SIZE);
				MakePacketCSMoveStart(&cWritePacket, _dwActionCur, _iCurX, _iCurY);
				SendPacket(&cWritePacket, dfPACKET_CS_MOVE_START);
			}
		}

		if (_iDirOld!=_iDirCur) {
			if (_iDirCur == LEFT) {
				SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
			}
			else {
				SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
			}
		}
	}
}
void CPlayerObject::SetActionStand() {
	if (_dwActionCur != dfACTION_STAND) {
		//서버에 보내기
		/*
		if (_dwActionCur < dfACTION_ATTACK1 || _dwActionCur > dfACTION_ATTACK3) {
			if (_bPlayerCharacter) {
				st_NETWORK_PACKET_HEADER Header;
				stPACKET_CS_MOVE_STOP Packet;
				MakePacketCSMoveStop(&Header, &Packet, _iDirCur, _iCurX, _iCurY);
				SendPacket(Header, (char*)&Packet);
			}
		}
		*/
		//직렬화 버퍼 적용
		if (_dwActionCur < dfACTION_ATTACK1 || _dwActionCur > dfACTION_ATTACK3) {
			if (_bPlayerCharacter) {
				CPacket cWritePacket(dfMAX_PACKET_BUFFER_SIZE);
				MakePacketCSMoveStop(&cWritePacket, _iDirCur, _iCurX, _iCurY);
				SendPacket(&cWritePacket, dfPACKET_CS_MOVE_STOP);
			}
		}
		_dwActionOld = _dwActionCur;
		_dwActionCur = _dwActionInput;
		if (_iDirCur == LEFT) {
			SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
		}
		else {
			SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
		}
	}
}

void CPlayerObject::SetDirection(int iDir) {
	_iDirCur = iDir;
}
void CPlayerObject::SetHP(char chHP) {
	_chHP = chHP;
}



//Effect
CEffectObject::CEffectObject(BOOL bEffectStart, DWORD dwAttackID, int iObjectID, int iObjectType) : CBaseObject(iObjectID, iObjectType) {
	_bEffectStart = bEffectStart;
	_dwAttackID = dwAttackID;
}
CEffectObject::~CEffectObject() {

}
BOOL CEffectObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch) {
	if (_bEffectStart > 0) {
		_bEffectStart--;
	}
	else {
		g_cSprite.DrawSprite(GetSprite(), _iCurX - g_CameraX, _iCurY - g_CameraY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	}
	return TRUE;
}
BOOL CEffectObject::Run(DWORD dwParam) {
	if (_bEffectStart > 0) {
		_bEffectStart--;
	}
	else {
		NextFrame();
	}

	return FALSE;
}
