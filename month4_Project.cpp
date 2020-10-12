// month4_Project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//


#include "framework.h"
#include "month4_Project.h"
#include "TCPNetwork.h"
using namespace std;


//네트워크 파트 정보
#define UM_NETWORK	(WM_USER+1)
WCHAR SERVERIP[16] = L"127.0.0.1";

unsigned short HeaderSize = 4;

SOCKET client_sock;
BOOL g_bConnect = FALSE;
BOOL g_bSend = FALSE;
BOOL g_bExit = FALSE;
BOOL g_bFile = FALSE;
WCHAR g_FileName[64];
CRingBuffer SendQ(1000);
CRingBuffer RecvQ(1000);
int g_CameraX = 0;
int g_CameraY = 0;
bool compare(CBaseObject* a, CBaseObject* b) {
	if (a->GetObjectType() == b->GetObjectType()) {
		return a->GetCurY() < b->GetCurY();
	}
	else {
		if (a->GetObjectType() == eTYPE_EFFECT)
			return false;
		else return true;
	}
}

CScreenDib g_cScreenDib(640, 480, 32);
CSpriteDib g_cSprite(65, 0x00ffffff);
CFrameSkip g_cFrameSkip(timeGetTime(), 20, 0, 0);


CBaseObject* g_cPlayerObject = NULL;

HWND g_hWnd;
BOOL g_bActiveAPP = FALSE;
HIMC g_hOldIMC;
list<CBaseObject*> ObjectList;



LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Update();
void Draw();
void KeyProcess();
void Action();
void sort(list<CBaseObject*>& List);
void SetCameraPos();
BOOL GameInit(void);

BOOL CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
	int retval;
	if (!GameInit())
		return -1;


	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MONTH4PROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MONTH4PROJECT);
	wcex.lpszClassName = L"MONTH4_PROJECT";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	g_hWnd = CreateWindowExW(0, L"MONTH4_PROJECT", L"TCPFighter", WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);

	if (!g_hWnd)
	{
		return FALSE;
	}

	if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), HWND_DESKTOP, MainDlgProc) == FALSE) {
		return -1;
	}

	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);
	SetFocus(g_hWnd);

	RECT WindowRect;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = 640;
	WindowRect.bottom = 480;

	AdjustWindowRectEx(&WindowRect, GetWindowStyle(g_hWnd), GetMenu(g_hWnd) != NULL, GetWindowExStyle(g_hWnd));

	//윈도우 생성 위치 조정
	int iX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (640 / 2);
	int iY = (GetSystemMetrics(SM_CXSCREEN) / 2) - (480 / 2);

	MoveWindow(g_hWnd, 0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, TRUE);

	//네트워크 설정

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return WSAGetLastError();
	}
	
	client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_sock == INVALID_SOCKET) {
		return WSAGetLastError();
	}

	WSAAsyncSelect(client_sock, g_hWnd, UM_NETWORK, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	InetPton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(dfNETWORK_PORT);

	retval = connect(client_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			return WSAGetLastError();
		}
	}

	//InitFile();



	MSG msg;

	// 기본 메시지 루프입니다:

	while (1) {
		if (g_bExit) break;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			if (!g_bConnect) continue;
			Update();
		}
	}

	MessageBox(g_hWnd, L"접속이 종료되었습니다.", L"끊김", MB_OK);

	list<CBaseObject*>::iterator it;
	for (it = ObjectList.begin(); it != ObjectList.end();) {
		delete (*it);
		it = ObjectList.erase(it);
	}
	WSACleanup();
	return (int)msg.wParam;
}


//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case UM_NETWORK: 
	{
		if (WSAGETSELECTERROR(lParam)) {
			//오류처리
			LogingError(WSAGETSELECTERROR(lParam), L"UM_NETWORK");
			return WSAGETSELECTERROR(lParam);
		}

		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_CONNECT:
			g_bConnect = TRUE;
			break;
		case FD_READ:
		{
			//ProcRead();

			//직렬화 버퍼 사용
			ProcRead();
		}
			break;
		case FD_WRITE:
			g_bSend = TRUE;
			ProcWrite();
			break;
		case FD_CLOSE:
			LogingError(WSAGETSELECTERROR(lParam), L"FD_CLOSE");
			break;
		}
	}
	break;
	case WM_CREATE:
		g_hOldIMC = ImmAssociateContext(hWnd, NULL);
		break;
	case WM_ACTIVATEAPP:
		g_bActiveAPP = (BOOL)wParam;
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		ImmAssociateContext(hWnd, g_hOldIMC);
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void Update() {
	if (g_bActiveAPP) {
		//키 입력 활성화

		KeyProcess();
	}
	Action();

	static int LogicCount = 0;
	static DWORD OldTime = 0;

	LogicCount++;
	if (timeGetTime() - OldTime >= 1000) {
		WCHAR TEXT[64];
		wsprintf(TEXT, L"LogicFrame:%d", LogicCount);
		SetWindowTextW(g_hWnd, TEXT);
		LogicCount = 0;
		OldTime = timeGetTime();
	}

	if (g_cFrameSkip.FrameSkip()) {
		Draw();
		g_cScreenDib.Flip(g_hWnd);
	}
}

BOOL GameInit(void) {

	/*g_cPlayerObject = new CPlayerObject(TRUE, 1, eTYPE_PLAYER);
	g_cPlayerObject->SetPosition(100, 100);
	g_cPlayerObject->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	((CPlayerObject*)g_cPlayerObject)->SetHP(100);
	((CPlayerObject*)g_cPlayerObject)->SetDirection(LEFT);

	CBaseObject* DumyPlayerObject1 = new CPlayerObject(TRUE, 1, eTYPE_PLAYER);
	DumyPlayerObject1->SetPosition(200, 200);
	DumyPlayerObject1->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	((CPlayerObject*)DumyPlayerObject1)->SetHP(100);
	((CPlayerObject*)DumyPlayerObject1)->SetDirection(LEFT);

	CBaseObject* DumyPlayerObject2 = new CPlayerObject(TRUE, 1, eTYPE_PLAYER);
	DumyPlayerObject2->SetPosition(400, 300);
	DumyPlayerObject2->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	((CPlayerObject*)DumyPlayerObject2)->SetHP(50);
	((CPlayerObject*)DumyPlayerObject2)->SetDirection(RIGHT);

	ObjectList.push_back(DumyPlayerObject1);
	ObjectList.push_back(DumyPlayerObject2);
	ObjectList.push_back(g_cPlayerObject);*/

	if (!g_cSprite.LoadDibSprite(eMAP, (WCHAR*)L"Sprite_Data\\Tile_01.bmp", 0, 0))
		return FALSE;

	//1~3 Stand_L
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_L01, (WCHAR*)L"Sprite_Data\\Stand_L_01.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_L02, (WCHAR*)L"Sprite_Data\\Stand_L_02.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_L_MAX, (WCHAR*)L"Sprite_Data\\Stand_L_03.bmp", 71, 90))
		return FALSE;

	//4~6 Stand_R
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_R01, (WCHAR*)L"Sprite_Data\\Stand_R_01.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_R02, (WCHAR*)L"Sprite_Data\\Stand_R_02.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_STAND_R_MAX, (WCHAR*)L"Sprite_Data\\Stand_R_03.bmp", 71, 90))
		return FALSE;

	//7~18 Move_L
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L01, (WCHAR*)L"Sprite_Data\\Move_L_01.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L02, (WCHAR*)L"Sprite_Data\\Move_L_02.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L03, (WCHAR*)L"Sprite_Data\\Move_L_03.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L04, (WCHAR*)L"Sprite_Data\\Move_L_04.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L05, (WCHAR*)L"Sprite_Data\\Move_L_05.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L06, (WCHAR*)L"Sprite_Data\\Move_L_06.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L07, (WCHAR*)L"Sprite_Data\\Move_L_07.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L08, (WCHAR*)L"Sprite_Data\\Move_L_08.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L09, (WCHAR*)L"Sprite_Data\\Move_L_09.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L10, (WCHAR*)L"Sprite_Data\\Move_L_10.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L11, (WCHAR*)L"Sprite_Data\\Move_L_11.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_L_MAX, (WCHAR*)L"Sprite_Data\\Move_L_12.bmp", 71, 90))
		return FALSE;

	//19~30 Move_R
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R01, (WCHAR*)L"Sprite_Data\\Move_R_01.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R02, (WCHAR*)L"Sprite_Data\\Move_R_02.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R03, (WCHAR*)L"Sprite_Data\\Move_R_03.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R04, (WCHAR*)L"Sprite_Data\\Move_R_04.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R05, (WCHAR*)L"Sprite_Data\\Move_R_05.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R06, (WCHAR*)L"Sprite_Data\\Move_R_06.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R07, (WCHAR*)L"Sprite_Data\\Move_R_07.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R08, (WCHAR*)L"Sprite_Data\\Move_R_08.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R09, (WCHAR*)L"Sprite_Data\\Move_R_09.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R10, (WCHAR*)L"Sprite_Data\\Move_R_10.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R11, (WCHAR*)L"Sprite_Data\\Move_R_11.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_MOVE_R_MAX, (WCHAR*)L"Sprite_Data\\Move_R_12.bmp", 71, 90))
		return FALSE;

	//31~34 Attack1_L
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L01, (WCHAR*)L"Sprite_Data\\Attack1_L_01.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L02, (WCHAR*)L"Sprite_Data\\Attack1_L_02.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L03, (WCHAR*)L"Sprite_Data\\Attack1_L_03.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L_MAX, (WCHAR*)L"Sprite_Data\\Attack1_L_04.bmp", 71, 90))
		return FALSE;

	//35~38 Attack1_R
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R01, (WCHAR*)L"Sprite_Data\\Attack1_R_01.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R02, (WCHAR*)L"Sprite_Data\\Attack1_R_02.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R03, (WCHAR*)L"Sprite_Data\\Attack1_R_03.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R_MAX, (WCHAR*)L"Sprite_Data\\Attack1_R_04.bmp", 71, 90))
		return FALSE;

	//39~42 Attack2_L
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L01, (WCHAR*)L"Sprite_Data\\Attack2_L_01.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L02, (WCHAR*)L"Sprite_Data\\Attack2_L_02.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L03, (WCHAR*)L"Sprite_Data\\Attack2_L_03.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L_MAX, (WCHAR*)L"Sprite_Data\\Attack2_L_04.bmp", 71, 90))
		return FALSE;

	//43~46 Attack2_R
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R01, (WCHAR*)L"Sprite_Data\\Attack2_R_01.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R02, (WCHAR*)L"Sprite_Data\\Attack2_R_02.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R03, (WCHAR*)L"Sprite_Data\\Attack2_R_03.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R_MAX, (WCHAR*)L"Sprite_Data\\Attack2_R_04.bmp", 71, 90))
		return FALSE;

	//47~52 Attack3_L
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L01, (WCHAR*)L"Sprite_Data\\Attack3_L_01.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L02, (WCHAR*)L"Sprite_Data\\Attack3_L_02.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L03, (WCHAR*)L"Sprite_Data\\Attack3_L_03.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L04, (WCHAR*)L"Sprite_Data\\Attack3_L_04.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L05, (WCHAR*)L"Sprite_Data\\Attack3_L_05.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L_MAX, (WCHAR*)L"Sprite_Data\\Attack3_L_06.bmp", 71, 90))
		return FALSE;

	//53~58 Attack3_R
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R01, (WCHAR*)L"Sprite_Data\\Attack3_R_01.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R02, (WCHAR*)L"Sprite_Data\\Attack3_R_02.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R03, (WCHAR*)L"Sprite_Data\\Attack3_R_03.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R04, (WCHAR*)L"Sprite_Data\\Attack3_R_04.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R05, (WCHAR*)L"Sprite_Data\\Attack3_R_05.bmp", 71, 90))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R_MAX, (WCHAR*)L"Sprite_Data\\Attack3_R_06.bmp", 71, 90))
		return FALSE;


	//59~62 xSpark
	if (!g_cSprite.LoadDibSprite(eEFFECT_SPARK_01, (WCHAR*)L"Sprite_Data\\xSpark_1.bmp", 70, 70))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(eEFFECT_SPARK_02, (WCHAR*)L"Sprite_Data\\xSpark_2.bmp", 70, 70))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(eEFFECT_SPARK_03, (WCHAR*)L"Sprite_Data\\xSpark_3.bmp", 70, 70))
		return FALSE;
	if (!g_cSprite.LoadDibSprite(eEFFECT_SPARK_MAX, (WCHAR*)L"Sprite_Data\\xSpark_4.bmp", 70, 70))
		return FALSE;

	//63 HP
	if (!g_cSprite.LoadDibSprite(eGUAGE_HP, (WCHAR*)L"Sprite_Data\\HPGuage.bmp", 0, 0))
		return FALSE;

	//64 Shadow
	if (!g_cSprite.LoadDibSprite(eSHADOW, (WCHAR*)L"Sprite_Data\\Shadow.bmp", 32, 4))
		return FALSE;

	return TRUE;

}

void KeyProcess() {
	if (g_cPlayerObject == NULL)
		return;
	DWORD dwAction = dfACTION_STAND;

	//LL
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 && GetAsyncKeyState(VK_LEFT) & 0x8000) {
		dwAction = dfACTION_MOVE_LL;
	}

	//UU
	if (GetAsyncKeyState(VK_UP) & 0x8000 && GetAsyncKeyState(VK_UP) & 0x8000) {
		dwAction = dfACTION_MOVE_UU;
	}

	//RR
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		dwAction = dfACTION_MOVE_RR;
	}

	//DD
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 && GetAsyncKeyState(VK_DOWN) & 0x8000) {
		dwAction = dfACTION_MOVE_DD;
	}

	//LU
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 && GetAsyncKeyState(VK_UP) & 0x8000) {
		dwAction = dfACTION_MOVE_LU;
	}

	//RU
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && GetAsyncKeyState(VK_UP) & 0x8000) {
		dwAction = dfACTION_MOVE_RU;
	}

	//RD
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && GetAsyncKeyState(VK_DOWN) & 0x8000) {
		dwAction = dfACTION_MOVE_RD;
	}

	//LD
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 && GetAsyncKeyState(VK_DOWN) & 0x8000) {
		dwAction = dfACTION_MOVE_LD;
	}

	//Attack1, Z버튼
	if (GetAsyncKeyState(0x5a) & 0x8000) {
		dwAction = dfACTION_ATTACK1;
	}

	//Attack2, X버튼
	if (GetAsyncKeyState(0x58) & 0x8000) {
		dwAction = dfACTION_ATTACK2;
	}

	//Attack3, C버튼
	if (GetAsyncKeyState(0x43) & 0x8000) {
		dwAction = dfACTION_ATTACK3;
	}

	g_cPlayerObject->ActionInput(dwAction);
}

void Action() {
	list<CBaseObject*>::iterator it;
	for (it = ObjectList.begin(); it != ObjectList.end();) {
		DWORD dwSprite = (*it)->GetSprite();

		////공격 시 Effect 생성
		//if (dwSprite == ePLAYER_ATTACK1_L01) {
		//	if ((*it)->GetDelayCount() == 0) {
		//		CBaseObject* Effect = new CEffectObject(1 * dfDELAY_ATTACK1, dfACTION_ATTACK1, 0, eTYPE_EFFECT);
		//		Effect->SetPosition((*it)->GetCurX() - 65, (*it)->GetCurY() - 70);
		//		Effect->SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_MAX, dfDELAY_EFFECT);
		//		ObjectList.push_back(Effect);
		//	}
		//}
		//else if (dwSprite == ePLAYER_ATTACK1_R01) {
		//	if ((*it)->GetDelayCount() == 0) {
		//		CBaseObject* Effect = new CEffectObject(1 * dfDELAY_ATTACK1, dfACTION_ATTACK1, 0, eTYPE_EFFECT);
		//		Effect->SetPosition((*it)->GetCurX() + 60, (*it)->GetCurY() - 70);
		//		Effect->SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_MAX, dfDELAY_EFFECT);
		//		ObjectList.push_back(Effect);
		//	}
		//}
		//else if (dwSprite == ePLAYER_ATTACK2_L01) {
		//	if ((*it)->GetDelayCount() == 0) {
		//		CBaseObject* Effect = new CEffectObject(2 * dfDELAY_ATTACK2, dfACTION_ATTACK2, 0, eTYPE_EFFECT);
		//		Effect->SetPosition((*it)->GetCurX() - 65, (*it)->GetCurY() - 65);
		//		Effect->SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_MAX, dfDELAY_EFFECT);
		//		ObjectList.push_back(Effect);
		//	}
		//}
		//else if (dwSprite == ePLAYER_ATTACK2_R01) {
		//	if ((*it)->GetDelayCount() == 0) {
		//		CBaseObject* Effect = new CEffectObject(dfDELAY_ATTACK2, dfACTION_ATTACK2, 0, eTYPE_EFFECT);
		//		Effect->SetPosition((*it)->GetCurX() + 60, (*it)->GetCurY() - 65);
		//		Effect->SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_MAX, dfDELAY_EFFECT);
		//		ObjectList.push_back(Effect);
		//	}
		//}
		//else if (dwSprite == ePLAYER_ATTACK3_L01) {
		//	if ((*it)->GetDelayCount() == 0) {
		//		CBaseObject* Effect = new CEffectObject(4 * dfDELAY_ATTACK3, dfACTION_ATTACK3, 0, eTYPE_EFFECT);
		//		Effect->SetPosition((*it)->GetCurX() - 65, (*it)->GetCurY() - 75);
		//		Effect->SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_MAX, dfDELAY_EFFECT);
		//		ObjectList.push_back(Effect);
		//	}
		//}
		//else if (dwSprite == ePLAYER_ATTACK3_R01) {
		//	if ((*it)->GetDelayCount() == 0) {
		//		CBaseObject* Effect = new CEffectObject(4 * dfDELAY_ATTACK3, dfACTION_ATTACK3, 0, eTYPE_EFFECT);
		//		Effect->SetPosition((*it)->GetCurX() + 65, (*it)->GetCurY() - 75);
		//		Effect->SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_MAX, dfDELAY_EFFECT);
		//		ObjectList.push_back(Effect);
		//	}
		//}


		if ((*it)->GetObjectType() == eTYPE_EFFECT) {
			(*it)->Run(NULL);
			if ((*it)->IsEndFrame()) {
				delete (*it);
				it = ObjectList.erase(it);
			}
			else {
				it++;
			}
		}
		else {
			(*it)->Run(NULL);
			it++;
		}
	}

	//리스트 정렬하기
	//sort(ObjectList);
	ObjectList.sort(compare);

}

void Draw() {


	BYTE* bypDest = g_cScreenDib.GetDibBuffer();
	int iDestWidth = g_cScreenDib.GetWidth();
	int iDestHeight = g_cScreenDib.GetHeight();
	int iDestPitch = g_cScreenDib.GetPitch();

	//맵 출력
	//카메라 좌표 구하기
	SetCameraPos();
	int TileX = g_CameraX - g_CameraX % 64;
	int TileY = g_CameraY - g_CameraY % 64;
	g_cSprite.DrawImage(eMAP, TileX- g_CameraX, TileY - g_CameraY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	list<CBaseObject*>::iterator it;
	for (it = ObjectList.begin(); it != ObjectList.end(); it++) {
		(*it)->Render(bypDest, iDestWidth, iDestHeight, iDestPitch);
	}




}

void SetCameraPos() {
	if (g_cPlayerObject == NULL) {
		g_CameraX = 0;
		g_CameraY = 0;
	}
	else {
		g_CameraX = max(0, g_cPlayerObject->GetCurX() -320);
		g_CameraY = max(0, g_cPlayerObject->GetCurY() - 240);
		g_CameraX = min(g_CameraX, 6400 - 640);
		g_CameraY = min(g_CameraY, 6400 - 480);

	}
}

void sort(list<CBaseObject*>& List) {
	list<CBaseObject*>::iterator it;
	list<CBaseObject*>::iterator it2;
	list<CBaseObject*>::iterator Tempit;
	for (it = List.begin(); it != List.end();) {
		Tempit = it;
		for (it2 = it; it2 != List.end(); it2++) {
			if ((*Tempit)->GetObjectType() == eTYPE_EFFECT && (*it2)->GetObjectType() == eTYPE_EFFECT) {
				if ((*it)->GetCurY() > (*it2)->GetCurY()) {
					Tempit = it2;
				}
			}
			else if ((*Tempit)->GetObjectType() == eTYPE_PLAYER && (*it2)->GetObjectType() == eTYPE_PLAYER) {
				if ((*it)->GetCurY() > (*it2)->GetCurY()) {
					Tempit = it2;
				}
			}
			else if ((*Tempit)->GetObjectType() == eTYPE_EFFECT && (*it2)->GetObjectType() == eTYPE_PLAYER) {
				Tempit = it2;
			}
		}
		if ((*Tempit) != (*it)) {
			List.push_front((*Tempit));
			List.erase(Tempit);
		}
		else {
			it++;
		}
	}
}

BOOL CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_STR, SERVERIP);
		return TRUE;
	case WM_COMMAND:
		switch (wParam) {
		case IDOK:
			GetDlgItemText(hDlg, IDC_STR, SERVERIP, 128);
			EndDialog(hDlg, 1);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return FALSE;
		}
	}
}

