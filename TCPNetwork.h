#pragma once

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <WS2tcpip.h>
#include "framework.h"
#include "month4_Project.h"
using namespace std;

extern unsigned short HeaderSize;

extern SOCKET client_sock;
extern BOOL g_bConnect;
extern BOOL g_bSend;
extern BOOL g_bExit;
extern BOOL g_bFile;
extern WCHAR g_FileName[64];
extern CRingBuffer SendQ;
extern CRingBuffer RecvQ;
extern CBaseObject* g_cPlayerObject;
extern list<CBaseObject*> ObjectList;

//네트워크 파트 함수들
//void SendPacket(st_NETWORK_PACKET_HEADER Header, char* Packet);
//직렬화버퍼 적용
void SendPacket(CPacket* cWritePacket, BYTE byType);
//void ProcRead();

//직렬화 버퍼 적용
void ProcRead();
void ProcWrite();


/*
void MakePacketCSMoveStart(st_NETWORK_PACKET_HEADER* Header, stPACKET_CS_MOVE_START* Packet,
	BYTE Dir, WORD X, WORD Y);
void MakePacketCSMoveStop(st_NETWORK_PACKET_HEADER* Header, stPACKET_CS_MOVE_STOP* Packet,
	BYTE Dir, WORD X, WORD Y);
void MakePacketCSAttack1(st_NETWORK_PACKET_HEADER* Header, stPACKET_CS_ATTACK1* Packet,
	BYTE Dir, WORD X, WORD Y);
void MakePacketCSAttack2(st_NETWORK_PACKET_HEADER* Header, stPACKET_CS_ATTACK2* Packet,
	BYTE Dir, WORD X, WORD Y);
void MakePacketCSAttack3(st_NETWORK_PACKET_HEADER* Header, stPACKET_CS_ATTACK3* Packet,
	BYTE Dir, WORD X, WORD Y);

*/

//직렬화 버퍼 적용
void MakePacketCSMoveStart(CPacket* cWritePacket, BYTE Dir, WORD X, WORD Y);
void MakePacketCSMoveStop(CPacket* cWritePacket, BYTE Dir, WORD X, WORD Y);
void MakePacketCSAttack1(CPacket* cWritePacket, BYTE Dir, WORD X, WORD Y);
void MakePacketCSAttack2(CPacket* cWritePacket, BYTE Dir, WORD X, WORD Y);
void MakePacketCSAttack3(CPacket* cWritePacket, BYTE Dir, WORD X, WORD Y);

/*
void CreateMyCharacter(char* Packet);
void CreateOtherCharacter(char* Packet);
void DeleteCharacter(char* Packet);
void MoveStart(char* Packet);
void MoveStop(char* Packet);
void Attack1(char* Packet);
void Attack2(char* Packet);
void Attack3(char* Packet);
void Damage(char* Packet);
*/

//직렬화 버퍼 적용
void SCCreateMyCharacter(CPacket* cReadPacket);
void SCCreateOtherCharacter(CPacket* cReadPacket);
void SCDeleteCharacter(CPacket* cReadPacket);
void SCMoveStart(CPacket* cReadPacket);
void SCMoveStop(CPacket* cReadPacket);
void SCAttack1(CPacket* cReadPacket);
void SCAttack2(CPacket* cReadPacket);
void SCAttack3(CPacket* cReadPacket);
void SCDamage(CPacket* cReadPacket);
void SCSync(CPacket* cReadPacket);

//void NetPacketProc(BYTE byPacketType, char* Packet);

//직렬화 버퍼 적용
void NetPacketProc(BYTE byPacketType, CPacket* cReadPacket);
CBaseObject* FindObject(DWORD ID);

void SetFileName(TCHAR* szFileName);

void InitFile();
void LogingToFile(int error, const WCHAR* FuncName);

void LogingError(int ErrCode, const WCHAR* function);
void LogingThrow(CPacket::EX* ex, const WCHAR* function);