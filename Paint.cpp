#include "framework.h"
#include "KPVLGraf.h"
#include "Paint.h"


LRESULT PaintCmd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
    EndPaint(hWnd, &ps);
    return 0;
}