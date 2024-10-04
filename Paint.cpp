#include "framework.h"
#include "KPVLGraf.h"
#include "Paint.h"


HWND hWndGraff = NULL;

//синяя заливка
HBRUSH TitleBrush2 = CreateSolidBrush(RGB(192, 192, 255));

std::string szGrafWindowClass = "GrafWindowClass";

LRESULT PaintCmd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
    EndPaint(hWnd, &ps);
    return 0;
}


LRESULT CALLBACK GrafWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_PAINT) return PaintCmd(hWnd, message, wParam, lParam);
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void MyRegisterClass()
{
    WNDCLASSEX cex0 ={0};
    cex0.cbSize = sizeof(WNDCLASSEX);
    cex0.style          = CS_HREDRAW | CS_VREDRAW;
    cex0.lpfnWndProc    = GrafWndProc;
    cex0.cbClsExtra     = 0;
    cex0.cbWndExtra     = 0;
    cex0.hInstance      = hInstance;
    cex0.hIcon          = NULL;
    cex0.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    cex0.hbrBackground  = TitleBrush2; // (HBRUSH)(CTLCOLOR_DLG + 1);
    cex0.lpszMenuName   = NULL;
    cex0.lpszClassName  = szGrafWindowClass.c_str();
    cex0.hIconSm        = NULL;

    if(cex0.hInstance && !RegisterClassExA(&cex0))
        throw std::exception(std::string("Ошибка регистрации класса окна cex0: " + std::string(szGrafWindowClass)).c_str());
}

void InitGraff()
{
        MyRegisterClass();
        RECT rc;
        GetClientRect(GlobalhWnd, &rc);
        //GetWindowRect(GlobalhWnd, &rc2);
        hWndGraff = CreateWindowEx(0/*WS_EX_TOPMOST*/,
                                  szGrafWindowClass.c_str(), "",
                                   WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_BORDER,
                                   5, 5, rc.right-10, 300,
                                   GlobalhWnd, (HMENU)100, hInstance, NULL);


        if(!hWndGraff) 
            throw std::exception((std::string(" Ошибка создания окна : " + szGrafWindowClass)).c_str());

}
