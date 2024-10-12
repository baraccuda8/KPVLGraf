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
#include "SQL.h"

#include "GdiPlusInit.h"
#include "Paint.h"


// Глобальные переменные:
bool isRun = false;;
HWND GlobalhWnd = NULL;
HINSTANCE hInstance = NULL;                                // текущий экземпляр
std::string szTitle = "KPVLGraf";                  // Текст строки заголовка
std::string szWindowClass = "KPVLGraf";            // имя класса главного окна
std::string CurrentDirPatch = "";
std::string strPatchFileName = "";


BOOL CenterWindow(HWND hwndChild, HWND hwndParent)
{
    RECT rcChild, rcParent;
    int  cxChild, cyChild, cxParent, cyParent, cxScreen, cyScreen, xNew, yNew;
    HDC  hdc;

    GetWindowRect(hwndChild, &rcChild);
    cxChild = rcChild.right - rcChild.left;
    cyChild = rcChild.bottom - rcChild.top;

    if(hwndParent)
    {
        GetWindowRect(hwndParent, &rcParent);
        cxParent = rcParent.right - rcParent.left;
        cyParent = rcParent.bottom - rcParent.top;
    }
    else
    {
        cxParent = GetSystemMetrics(SM_CXMAXIMIZED);
        cyParent = GetSystemMetrics(SM_CYMAXIMIZED);
        rcParent.left = 0;
        rcParent.top = 0;
    }

    hdc = GetDC(hwndChild);
    cxScreen = GetDeviceCaps(hdc, HORZRES);
    cyScreen = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(hwndChild, hdc);

    xNew = rcParent.left + (cxParent - cxChild) / 2;
    if(xNew < 0)xNew = 0; else if((xNew + cxChild) > cxScreen) xNew = cxScreen - cxChild;

    yNew = rcParent.top + (cyParent - cyChild) / 2;
    if(yNew < 0)yNew = 0; else if((yNew + cyChild) > cyScreen) yNew = cyScreen - cyChild;
    return SetWindowPos(hwndChild, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}




void DenugInfo(LOGLEVEL l, std::string f, std::string s1, std::string s2, std::string s3)
{
    time_t st = time(0);
    std::tm TM;
    localtime_s(&TM, &st);
    char sFormat[1024];
    if(l == LOGLEVEL::LEVEL_INFO)
        sprintf_s(sFormat, 1024, "[%04d-%02d-%02d %02d:%02d:%02d] [INFO] ", TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);
    else if(l == LOGLEVEL::LEVEL_WARN)
        sprintf_s(sFormat, 1024, "[%04d-%02d-%02d %02d:%02d:%02d] [WARN] ", TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);
    else if(l == LOGLEVEL::LEVEL_ERROR)
        sprintf_s(sFormat, 1024, "[%04d-%02d-%02d %02d:%02d:%02d] [ERROR] ", TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);
    else
        sprintf_s(sFormat, 1024, "[%04d-%02d-%02d %02d:%02d:%02d] [CRITICAL] ", TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);

    std::string file = f + ".log";
    std::ofstream F(file.c_str(), std::ios::binary | std::ios::out | std::ios::app);
    if(F.is_open())
    {
        F << sFormat << s1 << " -> " << s2 << " " << s3 << std::endl;
        F.close();
    }
}

//Вывод строки ошибки выполнения программы
int WinErrorExit(HWND hWnd, const char* lpszFunction)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s\r\nfailed with error %d:\r\n%s"), lpszFunction, dw, lpMsgBuf);

    //if(AllLogger) AllLogger->error(std::string((char*)lpDisplayBuf));
    //MessageBox(hWnd, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
    DenugInfo(LOGLEVEL::LEVEL_ERROR, AllLogger, FUNCTION_LINE_NAME, (char*)lpDisplayBuf);
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    PostQuitMessage(0);
    return 1;
}

LRESULT Quit()
{
    isRun = false;
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

void CurrentDir()
{
    char ss[256] = "";
    GetModuleFileNameA(NULL, ss, 255);
    char ss2[256];
    strcpy_s(ss2, 256, ss);
    char* s1 = ss2;
    char* s2 = NULL;
    while(s1 && *s1)
    {
        if(*s1 == '\\')s2 = s1;
        s1++;
    }
    if(s2)
    {
        *s2 = 0;
        SetCurrentDirectory(ss2);
        CurrentDirPatch = ss2;
    }
    strPatchFileName = std::string(ss);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    MSG msg;
    CurrentDir();
    try
    {
        hInstance = hInst;
        remove(AllLogger_Rem);


        if(!GdiPlusInit.Good())
            throw std::runtime_error("Not Init GdiPlus");


        MyRegisterClass(hInstance);
        if(!InitInstance ())
            throw std::runtime_error("!InitInstance");

        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KPVLGRAF));

        if(!InitSQL())
        {
            MessageBox(NULL, "Ошибка соединения с базой!", "Ошибка!", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);
            throw std::runtime_error("!InitSQL");
        }

        isRun = true;
        // Цикл основного сообщения:
        while(isRun && GetMessage(&msg, nullptr, 0, 0))
        {
            if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        isRun = false;
        StopSql();
    }CATCH(AllLogger, "Error:");

    return (int)0;
}

