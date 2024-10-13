#pragma once


namespace LoginDlg
{
	bool LoadConnect();
	void SaveConnect();
	INT_PTR CALLBACK bagSave(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};