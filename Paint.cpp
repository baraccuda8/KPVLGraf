#include "framework.h"
#include "KPVLGraf.h"
#include "Paint.h"


LRESULT PaintCmd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    // TODO: �������� ���� ����� ��� ����������, ������������ HDC...
    EndPaint(hWnd, &ps);
    return 0;
}