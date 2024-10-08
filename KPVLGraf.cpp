// KPVLGraf.cpp : Определяет точку входа для приложения.
//

#include "framework.h"

//#include <io.h>
//#include <strsafe.h>
//#include <shlobj.h>
//#include <stdlib.h>
//#include <direct.h>
//#include <wtsapi32.h>
//#include <psapi.h>
//#include <userenv.h>
//#include <chrono>
//#include <commctrl.h>
//#include <fstream>

#include "KPVLGraf.h"
#include "About.h"
#include "GdiPlusInit.h"
#include "Paint.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HWND GlobalhWnd = NULL;
HINSTANCE hInstance = NULL;                                // текущий экземпляр
std::string szTitle = "KPVLGraf";                  // Текст строки заголовка
std::string szWindowClass = "KPVLGraf";            // имя класса главного окна


LRESULT Quit()
{
    PostQuitMessage(0);
    return 0;
}


LRESULT CALLBACK Command(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId = LOWORD(wParam);
    // Разобрать выбор в меню:
    if(wmId == IDM_ABOUT) return DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
    if(wmId == IDM_EXIT) return DestroyWindow(hWnd);
    return DefWindowProc(hWnd, message, wParam, lParam);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_COMMAND)return Command(hWnd, message, wParam, lParam);
    if(message == WM_DESTROY)return Quit();
    return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL InitInstance()
{

    GlobalhWnd = CreateWindow(szWindowClass.c_str(), szTitle.c_str(), WS_OVERLAPPEDWINDOW | WS_MAXIMIZE, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if(!GlobalhWnd)
    {
        return FALSE;
    }


    ShowWindow(GlobalhWnd, SW_MAXIMIZE);
    UpdateWindow(GlobalhWnd);

    InitGraff();

    return TRUE;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX cex;

    cex.cbSize = sizeof(WNDCLASSEX);

    cex.style          = CS_HREDRAW | CS_VREDRAW;
    cex.lpfnWndProc    = WndProc;
    cex.cbClsExtra     = 0;
    cex.cbWndExtra     = 0;
    cex.hInstance      = hInstance;
    cex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KPVLGRAF));
    cex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    cex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    cex.lpszMenuName   = MAKEINTRESOURCE(IDC_KPVLGRAF);
    cex.lpszClassName  = szWindowClass.c_str();
    cex.hIconSm        = LoadIcon(cex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&cex);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    MSG msg;
    try
    {
        hInstance = hInst;

        if(!GdiPlusInit.Good())
            throw std::runtime_error("Not Init GdiPlus");


        MyRegisterClass(hInstance);
        if(!InitInstance ())
            throw std::runtime_error("!InitInstance");

        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KPVLGRAF));


        // Цикл основного сообщения:
        while(GetMessage(&msg, nullptr, 0, 0))
        {
            if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }CATCH("All Log", "Error:");

    return (int)0;
}

