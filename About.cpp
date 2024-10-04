#include "framework.h"
#include "KPVLGraf.h"
#include "About.h"



INT_PTR AboutComand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}
// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    if(message == WM_INITDIALOG) return (INT_PTR)TRUE;
    if(message == WM_COMMAND) return AboutComand(hDlg, wParam, lParam);

    return (INT_PTR)FALSE;
}
