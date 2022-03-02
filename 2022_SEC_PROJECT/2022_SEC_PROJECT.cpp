﻿// 2022_SEC_PROJECT.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "2022_SEC_PROJECT.h"
#include "RectControl.h"

#include<stdlib.h>
#include<time.h>

#define MAX_LOADSTRING 100

#define IDC_BTN_START 5000  //시작 버튼 ID
#define IDC_BTN_EDIT 5001  //설정 버튼 ID
#define IDC_BTN_EXIT 5002  //종료 버튼 ID

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

//내 변수들
//UI
HWND gameStartBtn, gameHelpBtn, gameExitBtn;
RECT menuClientRect, gameMenuRect;  // 사용가능 영역 크기
//게임시작용
///0 : 메인화면 / 1 : 노래 고르는 화면 / 2 : 게임 시작
int gameStart = 0;
//
int makeRectLeft, makeRectTop;  //제작용 타일 좌표
int mouseX, mouseY;  //마우스 위치
int score = 0; //점수
int check = 0;  //성공했나?
int tileAlphabet = 0; //0:없음, 1:A, 2:S, 3:D
int tileContinu = 0;  //타일이 어느형식으로 나오나?
int tileContinuCount = 0;  //타일이 몇번째 나온건가?
//
BOOL comboCheck = FALSE;  //콤보 체크용
BOOL doTileMake = FALSE;  //타일 만들어주나

//함수
void TimerInit(HWND timeHWND, int timeSet, int flag);  //타이머 초기화용 함수(flag 1 -> 맞음, 2 -> 틀림)
void TileMake();  //타일 만들어주기

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY2022SECPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2022SECPROJECT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2022SECPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY2022SECPROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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
RectControl* gameTileRect = new RectControl;

#define TIMER_ID_1      1  //타일용
#define TIMER_ID_2      2  //노래용

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int defaultTime; //타일용 타이머 시간설정
    switch (message)
    {
    case WM_CREATE:
        srand((unsigned int)time(NULL));
        SetWindowPos(hWnd, NULL, 500, 300, 1024, 768, 0);  // 게임창 크기 조절
        GetClientRect(hWnd, &menuClientRect);  // 조절된 크기 가져오기
        //실제 게임창
        gameMenuRect.left = menuClientRect.left;
        gameMenuRect.top = menuClientRect.top+50;
        gameMenuRect.right = menuClientRect.right - 150;
        gameMenuRect.bottom = menuClientRect.bottom;
        SetTimer(hWnd, TIMER_ID_1, 0, NULL);
        //버튼 3종
        gameStartBtn = CreateWindow(L"button", L"게 임  시 작", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            (menuClientRect.left + 50), 150, 150, 100, hWnd, (HMENU)IDC_BTN_START, NULL, NULL);
        gameHelpBtn = CreateWindow(L"button", L"설    정", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            (menuClientRect.left + 50), 300, 150, 100, hWnd, (HMENU)IDC_BTN_EDIT, NULL, NULL);
        gameExitBtn = CreateWindow(L"button", L"종    료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            (menuClientRect.left + 50), 450, 150, 100, hWnd, (HMENU)IDC_BTN_EXIT, NULL, NULL);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDC_BTN_START:
                MessageBox(hWnd, L"드갑니다!", L"게임시작", MB_OK);
                //가려주기
                ShowWindow(gameStartBtn, SW_HIDE);
                ShowWindow(gameHelpBtn, SW_HIDE);
                ShowWindow(gameExitBtn, SW_HIDE);
                tileContinuCount = 0;
                gameStart = 2;
                SetTimer(hWnd, TIMER_ID_2, 5000, NULL);
                InvalidateRect(hWnd, NULL, TRUE);
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
            case IDC_BTN_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_MOUSEMOVE:
        mouseX = LOWORD(lParam);
        mouseY = HIWORD(lParam);
        break;
    case WM_KEYDOWN:
        //41 : A, 53 : S, 44: D
        //기본 시간 1200, 연속 타일은 500
        defaultTime = 1500;
        if (tileContinu >= 2) {
            defaultTime = 500;
        }
        check = gameTileRect->CheckTile(mouseX, mouseY, makeRectLeft, makeRectTop, (makeRectLeft + 40), (makeRectTop + 40));
        switch (wParam)
        {
        case 0x41:
            if (tileAlphabet == 1) {
                //정답
                if (check == 1) {
                    score += 2;
                    TimerInit(hWnd, defaultTime, 1);
                }
                //틀림
                else if (check == 2) {
                    TimerInit(hWnd, defaultTime, 2);
                }
                //애매
                else if (check == 3) {
                    score++;
                    TimerInit(hWnd, defaultTime, 1);

                }
            }
            else {
                TimerInit(hWnd, defaultTime, 2);
            }
            break;
        case 0x53:
            if (tileAlphabet == 2) {
                //정답
                if (check == 1) {
                    score += 2;
                    TimerInit(hWnd, defaultTime, 1);
                }
                //틀림
                else if (check == 2) {
                    TimerInit(hWnd, defaultTime, 2);
                }
                //애매
                else if (check == 3) {
                    score++;
                    TimerInit(hWnd, defaultTime, 1);

                }
            }
            else {
                TimerInit(hWnd, defaultTime, 2);
            }
            break;
        case 0x44:
            if (tileAlphabet == 3) {
                //정답
                if (check == 1) {
                    score += 2;
                    TimerInit(hWnd, defaultTime, 1);
                }
                //틀림
                else if (check == 2) {
                    TimerInit(hWnd, defaultTime, 2);
                }
                //애매
                else if (check == 3) {
                    score++;
                    TimerInit(hWnd, defaultTime, 1);

                }
            }
            else {
                check = 2;
                TimerInit(hWnd, defaultTime, 2);
            }
            break;
        default:
            break;
        }
        doTileMake = TRUE;
        break;
    case WM_TIMER:
        if (TIMER_ID_1 == wParam && gameStart == 2) {
            defaultTime = 1500;
            if (tileContinu <= 2) {
                defaultTime = 500;
            }
            doTileMake = TRUE;
            KillTimer(hWnd, TIMER_ID_1);                    // 1번을 정지
            SetTimer(hWnd, TIMER_ID_1, defaultTime, NULL);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        /*
        else if (TIMER_ID_2 == wParam) {
            score += 500;
            KillTimer(hWnd, TIMER_ID_2);                    // 1번을 정지
            SetTimer(hWnd, TIMER_ID_2, 5000, NULL);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        */
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            HDC MemDC;
            HBITMAP myBitmap, oldBitmap;
            
            if (gameStart == 2) {

                Rectangle(hdc, gameMenuRect.left, gameMenuRect.top, gameMenuRect.right, gameMenuRect.bottom);
                if (doTileMake == TRUE) {
                    MemDC = CreateCompatibleDC(hdc);
                    TileMake();
                    //타일 색 바꿔주기
                    tileAlphabet = (rand() % 3) + 1;
                    myBitmap = LoadBitmap(hInst, MAKEINTATOM(IDB_BITMAP_TILE_A));
                    if (tileAlphabet == 1) {
                        myBitmap = LoadBitmap(hInst, MAKEINTATOM(IDB_BITMAP_TILE_A));
                    }
                    else if (tileAlphabet == 2) {
                        myBitmap = LoadBitmap(hInst, MAKEINTATOM(IDB_BITMAP_TILE_S));
                    }
                    else if (tileAlphabet == 3) {
                        myBitmap = LoadBitmap(hInst, MAKEINTATOM(IDB_BITMAP_TILE_D));
                    }
                    oldBitmap = (HBITMAP)SelectObject(MemDC, myBitmap);
                    BitBlt(hdc, makeRectLeft, makeRectTop, 40, 40, MemDC, 0, 0, SRCCOPY);  //비트맵 그려주기
                    SelectObject(MemDC, oldBitmap);
                    DeleteObject(myBitmap);
                    doTileMake = FALSE;
                }
                //Rectangle(hdc, makeRectLeft, makeRectTop, (makeRectLeft + 40), (makeRectTop + 40));
                
                WCHAR gameScoreText[32];
                wsprintfW(gameScoreText, L"점수 : %d", score);
                TextOut(hdc, menuClientRect.right - 80, 30, gameScoreText, lstrlenW(gameScoreText));
                //어느정도 정확도인지 써주기
                if (check == 1) {
                    wsprintfW(gameScoreText, L"Perfect Hit");
                    TextOut(hdc, menuClientRect.right - 80, 45, gameScoreText, lstrlenW(gameScoreText));
                }
                else if (check == 2) {
                    wsprintfW(gameScoreText, L"Miss");
                    TextOut(hdc, menuClientRect.right - 80, 45, gameScoreText, lstrlenW(gameScoreText));
                }
                else if (check == 3) {
                    wsprintfW(gameScoreText, L"Normal Hit");
                    TextOut(hdc, menuClientRect.right - 80, 45, gameScoreText, lstrlenW(gameScoreText));
                }
                check = 0;
                wsprintfW(gameScoreText, L"%d %d", gameMenuRect.right - gameMenuRect.left, gameMenuRect.bottom - gameMenuRect.top);
                TextOut(hdc, menuClientRect.right - 80, 75, gameScoreText, lstrlenW(gameScoreText));
            }
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void TimerInit(HWND timeHWND, int timeSet, int flag) {
    if (flag == 1) {
        KillTimer(timeHWND, TIMER_ID_1);
        SetTimer(timeHWND, TIMER_ID_1, timeSet, NULL);
        InvalidateRect(timeHWND, NULL, TRUE);
        comboCheck = TRUE;
    }
    else if (flag == 2) {
        KillTimer(timeHWND, TIMER_ID_1);
        SetTimer(timeHWND, TIMER_ID_1, timeSet, NULL);
        InvalidateRect(timeHWND, NULL, TRUE);
        comboCheck = FALSE;
    }
}

void TileMake() {
    
    //기본값
    if (tileContinu == 0) {
        //타일 좌표
        makeRectLeft = (rand() % gameMenuRect.right);
        makeRectTop = (rand() % gameMenuRect.bottom);
        tileContinu = (rand() % 5) + 1;
        //아래로 내리는거면 조금 올려주기(보정)
        if (tileContinu == 2) {
            makeRectTop -= 40;
        }
        //왼쪽으로 보정
        else if (tileContinu == 3) {
            makeRectLeft -= 40;
        }
    }
    //내려가는거
    if (tileContinu == 1) {
        if (tileContinuCount < 3) {
            if ((makeRectTop + 40) >= (gameMenuRect.bottom - 10)) {
                makeRectLeft -= 10;
            }
            else {
                makeRectTop += 10;
            }
            
        }
        else if (tileContinuCount == 3) {
            tileContinuCount = 0;
            tileContinu = 0;
            
        }
        if (makeRectLeft >= (gameMenuRect.right - 40)) {
            makeRectLeft = gameMenuRect.right - 45;
        }

        if ((makeRectTop + 40) >= (gameMenuRect.bottom - 10)) {
            makeRectTop = gameMenuRect.bottom - 45;
        }
        if (makeRectTop < gameMenuRect.top) {
            makeRectTop = gameMenuRect.top + 2;
        }
        tileContinuCount++;
    }
    //오른쪽
    else if (tileContinu == 2) {
        if (tileContinuCount < 4) {
            if (makeRectLeft >= (gameMenuRect.right - 40)) {
                makeRectTop -= 10;
            }
            else {
                makeRectLeft += 10;
            }
        }
        else if (tileContinuCount == 4) {
            tileContinuCount = 0;
            tileContinu = 0;

        }
        if (makeRectLeft >= (gameMenuRect.right - 40)) {
            makeRectLeft = gameMenuRect.right - 45;
        }

        if ((makeRectTop + 40) >= (gameMenuRect.bottom - 10)) {
            makeRectTop = gameMenuRect.bottom - 45;
        }
        if (makeRectTop < gameMenuRect.top) {
            makeRectTop = gameMenuRect.top + 2;
        }
        tileContinuCount++;
    }
    //1개 배치
    else {
        if (makeRectLeft >= (gameMenuRect.right - 40)) {
            makeRectLeft = gameMenuRect.right - 45;
        }

        if ((makeRectTop + 40) >= (gameMenuRect.bottom - 10)) {
            makeRectTop = gameMenuRect.bottom - 45;
        }
        if (makeRectTop < gameMenuRect.top) {
            makeRectTop = gameMenuRect.top + 2;
        }
        tileContinu = 0;
    }
}