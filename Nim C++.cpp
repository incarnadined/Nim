#include <windows.h>
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>

#define FILE_MENU_EXIT 15
#define CHANGE_TURN 16
#define PLAYER_1 17
#define PLAYER_2 18
#define FILE_MENU_HELP 19
#define FILE_MENU_RESTART 20
#define PERSONAL_MENU_NAMES 21
#define PERSONAL_MENU_COLOURS 22
#define PLAYER1_SUBMIT 23
#define PLAYER2_SUBMIT 24
#define changeColour1 25
#define changeColour2 26

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

void AddMenus(HWND);
void AddControls(HWND);
void LoadImages();

double NearestTriangleNumber(int);
int TriangleNumber(int);

void ReDraw(HWND, int);

void ChangeNames(HWND);
void NameSubmit(HWND, int);

DWORD ColourBox(HWND);

void ChangeTurn(HWND);

HMENU hMenu;
HMENU hFileMenu;
HMENU hPersonalisation;
HMENU hColours;

HWND hPlayer1;
HWND hPlayer2;
bool currentTurn = 0;
HWND hImgs[15];
HWND hNextTurn;
int help;

HBITMAP counter;

int exiting;
std::string out;
std::wstring stemp;

int WIDTH = 800;
int HEIGHT = 800;

std::vector< HWND > currentHit; // An arry that holds the values of the tiles that have been touched this turn

std::wstring nPlayer1 = L"Player 1";
std::wstring nPlayer2 = L"Player 2";
HWND ePlayer1; //Edit boxes for their names
HWND ePlayer2;
HWND sPlayer1; //Submit boxes for their names
HWND sPlayer2;

static DWORD rgbCurrent;

COLORREF black = RGB(0, 0, 0);
COLORREF cPlayer1 = RGB(255, 0, 0);
COLORREF cPlayer2 = RGB(0, 0, 255);
COLORREF a[1] = { black };
COLORREF b[2] = { black, black };
COLORREF c[3] = { black, black, black };
COLORREF d[4] = { black, black, black, black };
COLORREF e[5] = { black, black, black, black, black };
COLORREF *counters[5] = { a, b, c, d, e };

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	WNDCLASSW wc = { 0 };

	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"bored";
	wc.lpfnWndProc = WindowProc;

	if (!RegisterClassW(&wc))
		return -1;

	CreateWindowW(L"bored", L"Nim",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 200, WIDTH, HEIGHT,
		NULL, NULL, NULL, NULL);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wp))
		{
		case FILE_MENU_EXIT:
			exiting = MessageBox(NULL, L"Are you sure you want to exit?", L"Exit", MB_YESNOCANCEL);
			if (!(exiting == IDYES))
				break;
			DestroyWindow(hWnd);
			break;

		case FILE_MENU_HELP:
			help = MessageBox(hWnd, L"Nim is a 2-player game where the aim is to make your opponent take the last counter \n\
										To take a counter, click it to make its colour change. You can take as many counters as you want in your turn, but only from one row", L"Nim - Help",
				MB_OK | MB_ICONINFORMATION);
			if (help)

		case FILE_MENU_RESTART: {
				break;
			}

		case PERSONAL_MENU_NAMES:
			ChangeNames(hWnd);
			break;

		case CHANGE_TURN: {
			ChangeTurn(hWnd);
			break;
		}

		case changeColour1: 
			rgbCurrent = cPlayer1;
			cPlayer1 = ColourBox(hWnd);
			RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			break;

		case changeColour2:
			rgbCurrent = cPlayer2;
			cPlayer2 = ColourBox(hWnd);
			RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			break;

		case PLAYER1_SUBMIT:
			NameSubmit(hWnd, LOWORD(wp));

		case PLAYER2_SUBMIT:
			NameSubmit(hWnd, LOWORD(wp));

		default:
			if (HIWORD(wp) == BN_CLICKED) {
				NameSubmit(hWnd, LOWORD(wp));
			}

			if (HIWORD(wp) == STN_CLICKED && LOWORD(wp) < 16) {
				ReDraw(hWnd, LOWORD(wp));
				break;
			}

		break;
		}

	case WM_CTLCOLORSTATIC:
	{
		int testss = GetDlgCtrlID((HWND)lp);
		if (testss == PLAYER_1) { //If this message is being sent by the 'Player 1' text
			SetTextColor((HDC)wp, cPlayer1);
		}
		else if (testss == PLAYER_2) { //Or if it is being sent by 'Player 2'
			SetTextColor((HDC)wp, cPlayer2);
		}
		else if (testss < 16)//Or if it is a anything else (a counter)
			SetTextColor((HDC)wp, counters[(int)NearestTriangleNumber(testss) - 1][testss + 1 - TriangleNumber(NearestTriangleNumber(testss) - 1) - 1]);

		RECT rect = { WIDTH/2-100, 0, WIDTH / 2 + 100, 50 };
		HBRUSH brush;	
		if (!currentTurn)
			brush = CreateSolidBrush(cPlayer1);
		else
			brush = CreateSolidBrush(cPlayer2);
		FillRect(GetDC(hWnd), &rect, brush);
		DeleteObject(brush);


		return (BOOL)GetSysColorBrush(COLOR_MENU);
	}

	case WM_CREATE:
		LoadImages();
		AddMenus(hWnd);
		AddControls(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

void AddMenus(HWND hWnd)
{
	hMenu = CreateMenu();
	hFileMenu = CreateMenu();
	hPersonalisation = CreateMenu();
	hColours = CreateMenu();

	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_RESTART, L"Restart");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");

	AppendMenu(hPersonalisation, MF_STRING, PERSONAL_MENU_NAMES, L"Change Names");
	AppendMenu(hPersonalisation, MF_POPUP, (UINT_PTR)hColours, L"Change Colours");

	AppendMenu(hColours, MF_STRING, changeColour1, nPlayer1.c_str());
	AppendMenu(hColours, MF_STRING, changeColour2, nPlayer2.c_str());

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPersonalisation, L"Personalisation");
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)FILE_MENU_HELP, L"Help");

	SetMenu(hWnd, hMenu);
}

void AddControls(HWND hWnd)
{
	for (int i = 0; i < 15; ++i) {
		double n = NearestTriangleNumber(i);
		int counterX = ((WIDTH / (n + 1)) * (i + 1 - TriangleNumber(n - 1))) - 25;
		int counterY = (HEIGHT / 7) * n - 25; // 25 is the size of the image/2

		hImgs[i] = CreateWindowW(L"static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP | SS_NOTIFY,
			counterX, counterY, 100, 100, hWnd, (HMENU)i, NULL, NULL);
		SendMessageW(hImgs[i], STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)counter);
	}

	hPlayer1 = CreateWindowW(L"static", nPlayer1.c_str(), WS_VISIBLE | WS_CHILD | ES_CENTER,
		10, 10, 100, 50, hWnd, (HMENU)PLAYER_1, NULL, NULL);

	hPlayer2 = CreateWindowW(L"static", nPlayer2.c_str(), WS_VISIBLE | WS_CHILD | ES_CENTER,
		WIDTH - 120, 10, 100, 50, hWnd, (HMENU)PLAYER_2, NULL, NULL);

	hNextTurn = CreateWindowW(L"button", L"Next Turn", WS_VISIBLE | WS_CHILD | ES_CENTER,
		WIDTH / 2 - 50, HEIGHT - 150, 100, 50, hWnd, (HMENU)CHANGE_TURN, NULL, NULL);
}

void LoadImages()
{
	counter = (HBITMAP)LoadImage(NULL, L"G:/Coding Projects/Nim C++/assets/counter.bmp",
		IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);
}

double NearestTriangleNumber(int n)
{
	return floor((1+std::sqrt((8*n)+1))/2);
}

int TriangleNumber(int n)
{
	return n * (n + 1) / 2;
}

void ReDraw(HWND hWnd, int value)
{
	//Check if the player has the right to make the flip
	for (int i = 0; i < currentHit.size(); ++i)
	{
		int row = NearestTriangleNumber(GetDlgCtrlID(currentHit[i])); //Row that is active this turn
		if (NearestTriangleNumber(value) != row)
			return;
	}

	double n = NearestTriangleNumber(value);
	int counterX = ((WIDTH / (n + 1)) * (value + 1 - TriangleNumber(n - 1))) - 25;
	int counterY = (HEIGHT / 7) * n - 25; // 25 is the size of the image/2

	if (counters[(int)n - 1][value + 1 - TriangleNumber(n - 1) - 1] == black) {
		if (!currentTurn) //Player 1s turn
			counters[(int)n - 1][value + 1 - TriangleNumber(n - 1) - 1] = cPlayer1;
		else
			counters[(int)n - 1][value + 1 - TriangleNumber(n - 1) - 1] = cPlayer2;

		RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
		currentHit.push_back(hImgs[value]);
	}
	//Check if there are any more viable counters in a row to auto-end turn
	int valid = 0;
	for (int i = 0; i < (int)n; ++i) {
		if (counters[(int)n-1][i] == black) {
			++valid;
		}
	}
	if (valid == 0)
		ChangeTurn(hWnd);
}

void ChangeNames(HWND hWnd)
{
	ePlayer1 = CreateWindowW(L"edit", nPlayer1.c_str(), WS_VISIBLE | WS_CHILD | ES_CENTER,
		10, 70, 100, 15, hWnd, (HMENU)PLAYER_2, NULL, NULL);
	ePlayer2 = CreateWindowW(L"edit", nPlayer2.c_str(), WS_VISIBLE | WS_CHILD | ES_CENTER,
		WIDTH - 120, 70, 100, 15, hWnd, (HMENU)PLAYER_2, NULL, NULL);

	sPlayer1 = CreateWindowW(L"button", L"Change", WS_VISIBLE | WS_CHILD | ES_CENTER,
		10, 90, 100, 20, hWnd, (HMENU)PLAYER1_SUBMIT, NULL, NULL);
	sPlayer2 = CreateWindowW(L"button", L"Change", WS_VISIBLE | WS_CHILD | ES_CENTER,
		WIDTH - 120, 90, 100, 20, hWnd, (HMENU)PLAYER2_SUBMIT, NULL, NULL);
}

void NameSubmit(HWND hWnd, int player)
{
	if (player == PLAYER1_SUBMIT) {
		ShowWindow(ePlayer1, SW_HIDE);
		ShowWindow(sPlayer1, SW_HIDE);
		wchar_t wText[20];
		GetWindowText(ePlayer1, wText, 20);
		nPlayer1 = std::wstring(wText);

		hPlayer1 = CreateWindowW(L"static", nPlayer1.c_str(), WS_VISIBLE | WS_CHILD | ES_CENTER,
			10, 10, 100, 50, hWnd, (HMENU)PLAYER_1, NULL, NULL);
	}
	else if (player == PLAYER2_SUBMIT) {
		ShowWindow(ePlayer2, SW_HIDE);
		ShowWindow(sPlayer2, SW_HIDE);
		wchar_t wText[20];
		GetWindowText(ePlayer2, wText, 20);
		nPlayer2 = std::wstring(wText);

		hPlayer2 = CreateWindowW(L"static", nPlayer2.c_str(), WS_VISIBLE | WS_CHILD | ES_CENTER,
			WIDTH - 120, 10, 100, 50, hWnd, (HMENU)PLAYER_2, NULL, NULL);
	}
}

DWORD ColourBox(HWND hWnd)
{ 
	//https://docs.microsoft.com/en-us/windows/win32/dlgbox/using-common-dialog-boxes#choosing-a-color
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	HBRUSH hBrush;                  // brush handle

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hWnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE)
	{
		hBrush = CreateSolidBrush(cc.rgbResult);
		rgbCurrent = cc.rgbResult;
	}
	return rgbCurrent;
}

void ChangeTurn(HWND hWnd) {
	if (currentHit.size() == 0)
		return;
	if (currentTurn == 0)
		currentTurn = 1;
	else
		currentTurn = 0;

	hPlayer1 = CreateWindowW(L"static", nPlayer1.c_str(), WS_VISIBLE | WS_CHILD | ES_CENTER,
		10, 10, 100, 50, hWnd, (HMENU)PLAYER_1, NULL, NULL);

	hPlayer2 = CreateWindowW(L"static", nPlayer2.c_str(), WS_VISIBLE | WS_CHILD | ES_CENTER,
		WIDTH - 120, 10, 100, 50, hWnd, (HMENU)PLAYER_2, NULL, NULL);

	currentHit.clear();
}