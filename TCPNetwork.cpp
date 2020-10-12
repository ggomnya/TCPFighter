#include "TCPNetwork.h"


/*
void MakePacketCSMoveStart(st_PACKET_HEADER* Header, stPACKET_CS_MOVE_START* Packet,
	BYTE Dir, WORD X, WORD Y) {
	Header->byCode = dfPACKET_CODE;
	Header->bySize = sizeof(*Packet);
	Header->byType = dfPACKET_CS_MOVE_START;
	Packet->Direction = Dir;
	Packet->X = X;
	Packet->Y = Y;
}
void MakePacketCSMoveStop(st_PACKET_HEADER* Header, stPACKET_CS_MOVE_STOP* Packet,
	BYTE Dir, WORD X, WORD Y) {
	Header->byCode = dfPACKET_CODE;
	Header->bySize = sizeof(*Packet);
	Header->byType = dfPACKET_CS_MOVE_STOP;
	Packet->Direction = Dir;
	Packet->X = X;
	Packet->Y = Y;
}
void  MakePacketCSAttack1(st_PACKET_HEADER* Header, stPACKET_CS_ATTACK1* Packet,
	BYTE Dir, WORD X, WORD Y) {
	Header->byCode = dfPACKET_CODE;
	Header->bySize = sizeof(*Packet);
	Header->byType = dfPACKET_CS_ATTACK1;
	Packet->Direction = Dir;
	Packet->X = X;
	Packet->Y = Y;
}
void MakePacketCSAttack2(st_PACKET_HEADER* Header, stPACKET_CS_ATTACK2* Packet,
	BYTE Dir, WORD X, WORD Y) {
	Header->byCode = dfPACKET_CODE;
	Header->bySize = sizeof(*Packet);
	Header->byType = dfPACKET_CS_ATTACK2;
	Packet->Direction = Dir;
	Packet->X = X;
	Packet->Y = Y;
}
void MakePacketCSAttack3(st_PACKET_HEADER* Header, stPACKET_CS_ATTACK3* Packet,
	BYTE Dir, WORD X, WORD Y) {
	Header->byCode = dfPACKET_CODE;
	Header->bySize = sizeof(*Packet);
	Header->byType = dfPACKET_CS_ATTACK3;
	Packet->Direction = Dir;
	Packet->X = X;
	Packet->Y = Y;
}
*/

//직렬화 버퍼 적용
void MakePacketCSMoveStart(CPacket* cWritePacket, BYTE Dir, WORD X, WORD Y) {
	*cWritePacket << Dir;
	*cWritePacket << X;
	*cWritePacket << Y;
}
void MakePacketCSMoveStop(CPacket* cWritePacket, BYTE Dir, WORD X, WORD Y) {
	*cWritePacket << Dir;
	*cWritePacket << X;
	*cWritePacket << Y;
}
void  MakePacketCSAttack1(CPacket* cWritePacket, BYTE Dir, WORD X, WORD Y){
	*cWritePacket << Dir;
	*cWritePacket << X;
	*cWritePacket << Y;
}
void MakePacketCSAttack2(CPacket* cWritePacket, BYTE Dir, WORD X, WORD Y) {
	*cWritePacket << Dir;
	*cWritePacket << X;
	*cWritePacket << Y;
}
void MakePacketCSAttack3(CPacket* cWritePacket, BYTE Dir, WORD X, WORD Y) {
	*cWritePacket << Dir;
	*cWritePacket << X;
	*cWritePacket << Y;
}

/*
void SendPacket(st_PACKET_HEADER Header, char* Packet) {
	int retval = SendQ.Enqueue((char*)&Header, HeaderSize);
	if (retval < HeaderSize) {
		//오류 처리
		LogingError(0, L"SendPacket()_Header");
		return;
	}
	retval = SendQ.Enqueue(Packet, Header.bySize);
	if (retval < Header.bySize) {
		//오류 처리
		LogingError(0, L"SendPacket()_Payload");
		return;
	}
	BYTE EndCode = dfNETWORK_PACKET_END;
	retval = SendQ.Enqueue((char*)&EndCode, 1);
	if (retval < 1) {
		//오류처리
		LogingError(WSAGetLastError(), L"SendPacket()_Endcode");
		return;
	}
	if (g_bSend == FALSE) return;
	ProcWrite();

}
*/

//직렬화 버퍼 적용
void SendPacket(CPacket* cWritePacket, BYTE byType) {
	//헤더 만들기
	st_PACKET_HEADER stPacketHeader;
	stPacketHeader.byCode = dfPACKET_CODE;
	stPacketHeader.bySize = cWritePacket->GetDataSize();
	stPacketHeader.byType = byType;

	int retval = SendQ.Enqueue((char*)&stPacketHeader, sizeof(stPacketHeader));
	if (retval < sizeof(stPacketHeader)) {
		//오류처리
		LogingError(WSAGetLastError(), L"SendPacket()_Header");
		return;
	}
	//페이로드 넣기
	retval = SendQ.Enqueue(cWritePacket->GetBufferPtr(), cWritePacket->GetDataSize());
	if (retval < cWritePacket->GetDataSize()) {
		//오류처리
		LogingError(WSAGetLastError(), L"SendPacket()_Payload");
		return;
	}
	//EndCode 넣기
	BYTE EndCode = dfNETWORK_PACKET_END;
	retval = SendQ.Enqueue((char*)&EndCode, sizeof(EndCode));
	if (retval < sizeof(EndCode)) {
		//오류처리
		LogingError(WSAGetLastError(), L"SendPacket()_Endcode");
		return;
	}
	if (g_bSend == FALSE) return;
	ProcWrite();

}
/*
void ProcRead() {
	//주석 : 임시버퍼 활용, 기본 : 버퍼에 다이렉트로 접근
	//char RecvBuffer[5000];
	//int retval = recv(sock, RecvBuffer, 5000, 0);
	int retval = recv(client_sock, RecvQ.GetRearBufferPtr(), RecvQ.DirectEnqueueSize(), 0);
	if (retval == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			//오류처리
			LogingError(WSAGetLastError(), L"ProcRead()_recv()");
			return;
		}
		else
			return;
	}
	RecvQ.MoveRear(retval);
	//int RecvSize = RecvQ.Enqueue(RecvBuffer, retval);

	while (1) {
		//헤더 사이즈 이상이 있는지 확인
		if (RecvQ.GetUseSize() <= HeaderSize)
			break;

		//헤더 뽑기
		st_PACKET_HEADER Header;
		RecvQ.Peek((char*)&Header, HeaderSize);
		//byCode값 확인
		if (Header.byCode != dfPACKET_CODE) {
			//오류처리
			LogingError(WSAGetLastError(), L"ProcRead()_Header.byCode");
			return;
		}
		//페이로드, 엔드코드값이 있는지 확인
		if (RecvQ.GetUseSize() < (HeaderSize + Header.bySize + 1))
			break;

		RecvQ.MoveFront(HeaderSize);
		//페이로드 뽑기

		int retval;
		char* Packet = new char[Header.bySize];
		retval = RecvQ.Dequeue((char*)Packet, Header.bySize);
		if (retval < Header.bySize) {
			//오류처리
			LogingError(WSAGetLastError(), L"ProcRead()_RecvPayload");
			return;
		}
		NetPacketProc(Header.byType, Packet);
		delete Packet;
		//엔드코드 확인
		char EndCode;
		RecvQ.Peek((char*)&EndCode, 1);
		if (EndCode != dfNETWORK_PACKET_END) {
			//오류처리
			LogingError(WSAGetLastError(), L"ProcRead()_Endcode");
			return;
		}
		RecvQ.MoveFront(1);
	}
}
*/

//직렬화 버퍼 적용
void ProcRead() {
	int retval = recv(client_sock, RecvQ.GetRearBufferPtr(), RecvQ.DirectEnqueueSize(), 0);
	if (retval == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			//오류처리
			LogingError(WSAGetLastError(), L"ProcRead()_recv()");
			return;
		}
		else
			return;
	}
	RecvQ.MoveRear(retval);
	while (1) {
		//헤더 사이즈 이상이 있는지 확인
		if (RecvQ.GetUseSize() <= HeaderSize)
			break;


		//헤더 뽑기
		st_PACKET_HEADER Header;
		RecvQ.Peek((char*)&Header, HeaderSize);
		//byCode값 확인
		if (Header.byCode != dfPACKET_CODE) {
			//오류처리
			LogingError(WSAGetLastError(), L"ProcRead()_Header.byCode");
			return;
		}
		//페이로드, 엔드코드값이 있는지 확인
		if (RecvQ.GetUseSize() < (HeaderSize + Header.bySize + 1))
			break;

		RecvQ.MoveFront(HeaderSize);
		//페이로드 뽑기
		int retval;
		CPacket cReadPacket(dfMAX_PACKET_BUFFER_SIZE);
		retval = RecvQ.Dequeue(cReadPacket.GetBufferPtr(), Header.bySize);
		if (retval < Header.bySize) {
			//오류처리
			LogingError(WSAGetLastError(), L"ProcRead()_RecvPayload");
			return;
		}
		cReadPacket.MoveWritePos(retval);
		//엔드코드 확인
		BYTE EndCode;
		RecvQ.Peek((char*)&EndCode, 1);
		if (EndCode != dfNETWORK_PACKET_END) {
			//오류처리
			LogingError(WSAGetLastError(), L"ProcRead()_Endcode");
			return;
		}
		RecvQ.MoveFront(sizeof(EndCode));

		try {
			NetPacketProc(Header.byType, &cReadPacket);
		}
		catch (CPacket::EX* ex) {
			LogingThrow(ex, L"try_catch");
			//LogingError(WSAGetLastError(), L"throw error");
			delete ex;
			return;
		}
		
	}
}

void ProcWrite() {
	//주석 : 임시버퍼 활용, 기본 : 버퍼에 다이렉트로 접근
	while (1) {
		if (SendQ.GetUseSize() <= 0)
			return;
		if (g_bSend == FALSE)
			return;
		/*
		char SendBuffer[5000];
		int sendSize = SendQ.Peek(SendBuffer, 5000);
		int retval = send(sock, SendBuffer, sendSize, 0);
		*/
		int retval = send(client_sock, SendQ.GetFrontBufferPtr(), SendQ.DirectDequeueSize(), 0);
		if (retval == SOCKET_ERROR) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				//오류처리
				LogingError(WSAGetLastError(), L"ProcWrite()_send()");
				return;
			}
			else {
				g_bSend = FALSE;
				return;
			}
		}
		SendQ.MoveFront(retval);
	}
}
/*
void CreateMyCharacter(char* Packet) {
	stPACKET_SC_CREATE_MY_CHARACTER* pPacket = (stPACKET_SC_CREATE_MY_CHARACTER*)Packet;
	g_cPlayerObject = new CPlayerObject(TRUE, pPacket->ID, eTYPE_PLAYER);
	g_cPlayerObject->SetPosition(pPacket->X, pPacket->Y);
	if (pPacket->Direction == LEFT)
		g_cPlayerObject->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	else
		g_cPlayerObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	((CPlayerObject*)g_cPlayerObject)->SetHP(pPacket->HP);
	((CPlayerObject*)g_cPlayerObject)->SetDirection(pPacket->Direction);
	ObjectList.push_back(g_cPlayerObject);
}

void CreateOtherCharacter(char* Packet) {
	stPACKET_SC_CREATE_OTHER_CHARACTER* pPacket = (stPACKET_SC_CREATE_OTHER_CHARACTER*)Packet;
	CBaseObject* OtherPlayerObject = new CPlayerObject(FALSE, pPacket->ID, eTYPE_PLAYER);
	OtherPlayerObject->SetPosition(pPacket->X, pPacket->Y);
	if (pPacket->Direction == LEFT)
		OtherPlayerObject->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	else
		OtherPlayerObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	((CPlayerObject*)OtherPlayerObject)->SetHP(pPacket->HP);
	((CPlayerObject*)OtherPlayerObject)->SetDirection(pPacket->Direction);
	ObjectList.push_back(OtherPlayerObject);
}

void DeleteCharacter(char* Packet) {
	stPACKET_SC_DELETE_CHARACTER* pPacket = (stPACKET_SC_DELETE_CHARACTER*)Packet;
	list<CBaseObject*>::iterator it;
	for (it = ObjectList.begin(); it != ObjectList.end();) {
		if ((*it)->GetObjectID() == pPacket->ID) {
			delete (*it);
			ObjectList.erase(it);
			break;
		}
		it++;
	}
}

void MoveStart(char* Packet) {
	stPACKET_SC_MOVE_START* pPacket = (stPACKET_SC_MOVE_START*)Packet;
	CPlayerObject* Obj = (CPlayerObject*)FindObject(pPacket->ID);
	if (Obj == NULL) return;
	Obj->SetPosition(pPacket->X, pPacket->Y);
	Obj->ActionInput(pPacket->Direction);
}

void MoveStop(char* Packet) {
	stPACKET_SC_MOVE_STOP* pPacket = (stPACKET_SC_MOVE_STOP*)Packet;
	CPlayerObject* Obj = (CPlayerObject*)FindObject(pPacket->ID);
	if (Obj == NULL) return;
	Obj->SetPosition(pPacket->X, pPacket->Y);
	Obj->ActionInput(dfACTION_STAND);
}

void Attack1(char* Packet) {
	stPACKET_SC_ATTACK1* pPacket = (stPACKET_SC_ATTACK1*)Packet;
	CPlayerObject* Obj = (CPlayerObject*)FindObject(pPacket->ID);
	if (Obj == NULL) return;
	Obj->SetPosition(pPacket->X, pPacket->Y);
	Obj->SetDirection(pPacket->Direction);
	Obj->ActionInput(dfACTION_ATTACK1);
}
void Attack2(char* Packet) {
	stPACKET_SC_ATTACK2* pPacket = (stPACKET_SC_ATTACK2*)Packet;
	CPlayerObject* Obj = (CPlayerObject*)FindObject(pPacket->ID);
	if (Obj == NULL) return;
	Obj->SetPosition(pPacket->X, pPacket->Y);
	Obj->SetDirection(pPacket->Direction);
	Obj->ActionInput(dfACTION_ATTACK2);
}
void Attack3(char* Packet) {
	stPACKET_SC_ATTACK3* pPacket = (stPACKET_SC_ATTACK3*)Packet;
	CPlayerObject* Obj = (CPlayerObject*)FindObject(pPacket->ID);
	if (Obj == NULL) return;
	Obj->SetPosition(pPacket->X, pPacket->Y);
	Obj->SetDirection(pPacket->Direction);
	Obj->ActionInput(dfACTION_ATTACK3);
}

void Damage(char* Packet) {
	stPACKET_SC_DAMAGE* pPacket = (stPACKET_SC_DAMAGE*)Packet;
	CPlayerObject* AttObj = (CPlayerObject*)FindObject(pPacket->AttackID);
	CPlayerObject* DmgObj = (CPlayerObject*)FindObject(pPacket->DamageID);
	if (AttObj == NULL || DmgObj == NULL) return;
	DmgObj->SetHP(pPacket->DamageHP);
	CBaseObject* Effect = new CEffectObject(2 * AttObj->GetCurAction() -5, pPacket->AttackID, 0, eTYPE_EFFECT);
	Effect->SetPosition(DmgObj->GetCurX(), DmgObj->GetCurY() - 50);
	Effect->SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_MAX, dfDELAY_EFFECT);
	ObjectList.push_back(Effect);
}
*/

//직렬화 버퍼 적용
void SCCreateMyCharacter(CPacket* cReadPacket) {
	DWORD dwID;
	BYTE byDirection;
	WORD wX;
	WORD wY;
	BYTE byHP;
	*cReadPacket >> dwID >> byDirection >> wX >> wY >> byHP;

	g_cPlayerObject = new CPlayerObject(TRUE, dwID, eTYPE_PLAYER);
	g_cPlayerObject->SetPosition(wX, wY);
	if (byDirection == LEFT)
		g_cPlayerObject->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	else
		g_cPlayerObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	((CPlayerObject*)g_cPlayerObject)->SetHP(byHP);
	((CPlayerObject*)g_cPlayerObject)->SetDirection(byDirection);
	ObjectList.push_back(g_cPlayerObject);
}

void SCCreateOtherCharacter(CPacket* cReadPacket) {
	DWORD dwID;
	BYTE byDirection;
	WORD wX;
	WORD wY;
	BYTE byHP;
	*cReadPacket >> dwID >> byDirection >> wX >> wY >> byHP;

	CBaseObject* OtherPlayerObject = new CPlayerObject(FALSE, dwID, eTYPE_PLAYER);
	OtherPlayerObject->SetPosition(wX, wY);
	if (byDirection == LEFT)
		OtherPlayerObject->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	else
		OtherPlayerObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	((CPlayerObject*)OtherPlayerObject)->SetHP(byHP);
	((CPlayerObject*)OtherPlayerObject)->SetDirection(byDirection);
	ObjectList.push_back(OtherPlayerObject);
}

void SCDeleteCharacter(CPacket* cReadPacket) {
	DWORD dwID;
	*cReadPacket >> dwID;
	list<CBaseObject*>::iterator it;
	for (it = ObjectList.begin(); it != ObjectList.end();) {
		if ((*it)->GetObjectID() == dwID) {
			delete (*it);
			ObjectList.erase(it);
			break;
		}
		it++;
	}
}

void SCMoveStart(CPacket* cReadPacket) {
	DWORD dwID;
	BYTE byDirection;
	WORD wX;
	WORD wY;
	*cReadPacket >> dwID >> byDirection >> wX >> wY;
	CPlayerObject* Obj = (CPlayerObject*)FindObject(dwID);
	if (Obj == NULL) return;
	Obj->SetPosition(wX, wY);
	Obj->ActionInput(byDirection);
}

void SCMoveStop(CPacket* cReadPacket) {
	DWORD dwID;
	BYTE byDirection;
	WORD wX;
	WORD wY;
	*cReadPacket >> dwID >> byDirection >> wX >> wY;
	CPlayerObject* Obj = (CPlayerObject*)FindObject(dwID);
	if (Obj == NULL) return;
	Obj->SetPosition(wX, wY);
	Obj->ActionInput(dfACTION_STAND);
}

void SCAttack1(CPacket* cReadPacket) {
	DWORD dwID;
	BYTE byDirection;
	WORD wX;
	WORD wY;
	*cReadPacket >> dwID >> byDirection >> wX >> wY;
	CPlayerObject* Obj = (CPlayerObject*)FindObject(dwID);
	if (Obj == NULL) return;
	Obj->SetPosition(wX, wY);
	Obj->SetDirection(byDirection);
	Obj->ActionInput(dfACTION_ATTACK1);
}
void SCAttack2(CPacket* cReadPacket) {
	DWORD dwID;
	BYTE byDirection;
	WORD wX;
	WORD wY;
	*cReadPacket >> dwID >> byDirection >> wX >> wY;
	CPlayerObject* Obj = (CPlayerObject*)FindObject(dwID);
	if (Obj == NULL) return;
	Obj->SetPosition(wX, wY);
	Obj->SetDirection(byDirection);
	Obj->ActionInput(dfACTION_ATTACK2);
}
void SCAttack3(CPacket* cReadPacket) {
	DWORD dwID;
	BYTE byDirection;
	WORD wX;
	WORD wY;
	*cReadPacket >> dwID >> byDirection >> wX >> wY;
	CPlayerObject* Obj = (CPlayerObject*)FindObject(dwID);
	if (Obj == NULL) return;
	Obj->SetPosition(wX, wY);
	Obj->SetDirection(byDirection);
	Obj->ActionInput(dfACTION_ATTACK3);
}

void SCDamage(CPacket* cReadPacket) {
	DWORD dwAttackID;
	DWORD dwDamageID;
	BYTE byDamageHP;
	*cReadPacket >> dwAttackID >> dwDamageID >> byDamageHP;
	CPlayerObject* AttObj = (CPlayerObject*)FindObject(dwAttackID);
	CPlayerObject* DmgObj = (CPlayerObject*)FindObject(dwDamageID);
	if (DmgObj != NULL) {
		DmgObj->SetHP(byDamageHP);
	}
	if (DmgObj != NULL && AttObj != NULL) {
		CBaseObject* Effect = new CEffectObject(2 * AttObj->GetCurAction() - 5, dwAttackID, 0, eTYPE_EFFECT);
		Effect->SetPosition(DmgObj->GetCurX(), DmgObj->GetCurY() - 50);
		Effect->SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_MAX, dfDELAY_EFFECT);
		ObjectList.push_back(Effect);
	}

}

void SCSync(CPacket* cReadPacket) {
	DWORD dwID;
	WORD wX;
	WORD wY;
	*cReadPacket >> dwID >> wX >> wY;
	CPlayerObject* PlayerObj = (CPlayerObject*)FindObject(dwID);
	if (PlayerObj != NULL) {
		PlayerObj->SetPosition(wX, wY);
	}
}

/*
void NetPacketProc(BYTE byPacketType, char* Packet) {
	switch (byPacketType) {
	case dfPACKET_SC_CREATE_MY_CHARACTER:
		CreateMyCharacter(Packet);
		break;
	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
		CreateOtherCharacter(Packet);
		break;
	case dfPACKET_SC_DELETE_CHARACTER:
		DeleteCharacter(Packet);
		break;
	case dfPACKET_SC_MOVE_START:
		MoveStart(Packet);
		break;
	case dfPACKET_SC_MOVE_STOP:
		MoveStop(Packet);
		break;
	case dfPACKET_SC_ATTACK1:
		Attack1(Packet);
		break;
	case dfPACKET_SC_ATTACK2:
		Attack2(Packet);
		break;
	case dfPACKET_SC_ATTACK3:
		Attack3(Packet);
		break;
	case dfPACKET_SC_DAMAGE:
		Damage(Packet);
		break;
	default:
		//오류처리
		LogingError(WSAGetLastError(), L"ProcRead()_default");
		return;
	}
}
*/

//직렬화 버퍼 적용
void NetPacketProc(BYTE byPacketType, CPacket* cReadPacket) {
	switch (byPacketType) {
	case dfPACKET_SC_CREATE_MY_CHARACTER:
		SCCreateMyCharacter(cReadPacket);
		break;
	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
		SCCreateOtherCharacter(cReadPacket);
		break;
	case dfPACKET_SC_DELETE_CHARACTER:
		SCDeleteCharacter(cReadPacket);
		break;
	case dfPACKET_SC_MOVE_START:
		SCMoveStart(cReadPacket);
		break;
	case dfPACKET_SC_MOVE_STOP:
		SCMoveStop(cReadPacket);
		break;
	case dfPACKET_SC_ATTACK1:
		SCAttack1(cReadPacket);
		break;
	case dfPACKET_SC_ATTACK2:
		SCAttack2(cReadPacket);
		break;
	case dfPACKET_SC_ATTACK3:
		SCAttack3(cReadPacket);
		break;
	case dfPACKET_SC_DAMAGE:
		SCDamage(cReadPacket);
		break;
	case dfPACKET_SC_SYNC:
		SCSync(cReadPacket);
		break;
	default:
		//오류처리
		LogingError(WSAGetLastError(), L"ProcRead()_default");
		return;
	}
}
CBaseObject* FindObject(DWORD ID) {
	list<CBaseObject*>::iterator it;
	for (it = ObjectList.begin(); it != ObjectList.end();) {
		if ((*it)->GetObjectID() == ID) {
			return (*it);
		}
		it++;
	}
	return NULL;
}

void SetFileName(TCHAR* szFileName) {
	tm TM;
	time_t t;
	time(&t);
	localtime_s(&TM, &t);
	StringCchPrintfW(szFileName, 64, _T("Log_%04d%02d%02d_%02d%02d%02d.txt"), TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);
}

void InitFile() {
	FILE* fp;
	SetFileName(g_FileName);
	_wfopen_s(&fp, g_FileName, L"wb");
	fclose(fp);
}

void LogingToFile(int error, const WCHAR* FuncName) {
	tm TM;
	time_t t;
	time(&t);
	localtime_s(&TM, &t);
	FILE* fp;
	_wfopen_s(&fp, g_FileName, L"ab");
	//fwprintf_s(fp, L"%d %d", g_cPlayerObject->GetCurX(), g_cPlayerObject->GetCurY());
	fwprintf_s(fp, L"ErrorCode() : %d Time : %04d.%02d.%02d_%02d:%02d:%02d Function : %s\n", error, TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec, FuncName);
	fclose(fp);
}

void LogingError(int ErrCode, const WCHAR* function) {
	if (!g_bFile) {
		InitFile();
		g_bFile = TRUE;
	}
	LogingToFile(ErrCode, function);
	g_bExit = TRUE;
	closesocket(client_sock);
}

void LogingThrow(CPacket::EX* ex, const WCHAR* function) {
	if (!g_bFile) {
		InitFile();
		g_bFile = TRUE;
	}
	tm TM;
	time_t t;
	time(&t);
	localtime_s(&TM, &t);
	FILE* fp;
	_wfopen_s(&fp, g_FileName, L"ab");
	fwrite(ex->_Buffer, CPacket::eBUFFER_DEFAULT, 1, fp);
	fclose(fp);
	g_bExit = TRUE;
	closesocket(client_sock);
}