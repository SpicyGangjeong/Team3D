#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "MainApp.h"
#include "GameInstance.h"

#define MAX_LOADSTRING 100

HINSTANCE   g_hInstance;
HWND        g_hWnd;
WCHAR       szTitle[MAX_LOADSTRING];
WCHAR       szWindowClass[MAX_LOADSTRING];
_bool       g_bFullscreen = false;
RECT        g_rcWindowed = {}; 



ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                ToggleFullscreen();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(45489776);
    //_CrtSetBreakAlloc(1182);
    //_CrtSetBreakAlloc(1183);
    //_CrtSetBreakAlloc(1184);
    //_CrtSetBreakAlloc(1185);
    //_CrtSetBreakAlloc(1186);
    //_CrtSetBreakAlloc(1187);
#endif
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    CMainApp* pMainApp = { nullptr };

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));
    
    MSG msg;

    pMainApp = CMainApp::Create();
    if (nullptr == pMainApp){
        return FALSE;
    }

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    SAFE_ADDREF(pGameInstance);

    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Default")))) {
        return E_FAIL;
    }
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_60")))) {
        return E_FAIL;
    }

#ifdef _DEBUG
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_PriorityUpdate")))) {
        return E_FAIL;
    }
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Update")))) {
        return E_FAIL;
    }
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_LateUpdate")))) {
        return E_FAIL;
    }
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_DrawCall")))) {
        return E_FAIL;
    }
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Present")))) {
        return E_FAIL;
    }
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Picking")))) {
        return E_FAIL;
    }
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_PhysX")))) {
        return E_FAIL;
    }
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Level")))) {
        return E_FAIL;
    }
    if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_FrameCount")))) {
        return E_FAIL;
    }
#endif // _DEBUG


    _float      fTimeAcc = { };
    const _float fMaxTimer = { 1.f / 60.f };
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == msg.message){
                break;
            }
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        pGameInstance->Compute_TimeDelta(TEXT("Timer_Default"));

        fTimeAcc += pGameInstance->Get_TimeDelta(TEXT("Timer_Default"));

        if (fTimeAcc >= 1.f / 60.f)
        {
#ifdef _DEBUG
            pGameInstance->Compute_TimeDelta(TEXT("Timer_FrameCount"));
#endif // _DEBUG

            pGameInstance->Compute_TimeDelta(TEXT("Timer_60"));

#ifdef _DEBUG
            pMainApp->Update(1.f / 60.f);
#endif // _DEBUG
#ifndef _DEBUG
            _float fFrameTime = min(pGameInstance->Get_TimeDelta(TEXT("Timer_60")), fMaxTimer);
            pMainApp->Update(fFrameTime);
#endif
            pMainApp->Render();

            fTimeAcc = 0.f;
#ifdef _DEBUG
            pGameInstance->Compute_TimeDelta(TEXT("Timer_FrameCount"));
            pMainApp->Compute_FrameCount();
#endif // _DEBUG
        }
    }

    SAFE_RELEASE(pGameInstance);
    SAFE_RELEASE(pMainApp);

    return (int)msg.wParam;
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInstance = hInstance;
    RECT     rcWindow = { 0, 0, g_iWinSizeX, g_iWinSizeY };

    AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    g_hWnd = hWnd;

//#ifndef _DEBUG
//    {
//        g_bFullscreen = true;
//
//        SetWindowLong(g_hWnd, GWL_STYLE, GetWindowLong(g_hWnd, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
//
//        _int screenX = GetSystemMetrics(SM_CXSCREEN);
//        _int screenY = GetSystemMetrics(SM_CYSCREEN);
//
//        SetWindowPos(
//            g_hWnd,
//            HWND_TOP,
//            0, 0,
//            screenX, screenY,
//            SWP_NOOWNERZORDER | SWP_FRAMECHANGED
//        );
//    }
//#endif


    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
        return true;
    }
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SYSKEYDOWN:
    {
        if (wParam == VK_RETURN && (lParam & (1 << 29)))
        {
            ToggleFullscreen();
            return 0;
        }
    }
    break;


    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void ToggleFullscreen()
{
    DWORD style = GetWindowLong(g_hWnd, GWL_STYLE);

    if (!g_bFullscreen)
    {
        GetWindowRect(g_hWnd, &g_rcWindowed);

        SetWindowLong(g_hWnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);

        _int screenX = GetSystemMetrics(SM_CXSCREEN);
        _int screenY = GetSystemMetrics(SM_CYSCREEN);

        SetWindowPos(
            g_hWnd,
            HWND_TOP,
            0, 0,
            screenX, screenY,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED
        );

        g_bFullscreen = true;
    }
    else
    {
        SetWindowLong(g_hWnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);

        SetWindowPos(
            g_hWnd,
            HWND_NOTOPMOST,
            g_rcWindowed.left,
            g_rcWindowed.top,
            g_rcWindowed.right - g_rcWindowed.left,
            g_rcWindowed.bottom - g_rcWindowed.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED
        );

        g_bFullscreen = false;
    }
}
