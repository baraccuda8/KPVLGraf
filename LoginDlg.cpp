#include "framework.h"
#include "KPVLGraf.h"

#include "SQL.h"
#include "LoginDlg.h"

namespace LoginDlg
{
#define SQLFileName "PostgreSQL.dat"

    std::string pKey = "õàáðàbarracudabarracudaõàáðà";

    char sCommand2[0xFFFF];

    void encode(char* pText, size_t len)
    {
        for(size_t i = 0; i < len; i++)
            pText[i] = (byte)(pText[i] ^ pKey[i % pKey.length()]);
    }

    bool LoadConnect()
    {
        memset(sCommand2, 0, 0xFFFF);
        std::ifstream s(SQLFileName, std::ios::binary | std::ios::in);
        if(s.is_open())
        {
            s.read(sCommand2, 1024);
            int len = (int)s.gcount();
            s.close();
            encode(sCommand2, len);
            std::vector <std::string>split;
            boost::split(split, sCommand2, boost::is_any_of("\n"));
            if(split.size() == 5)
            {
                m_dbhost = split[0];
                m_dbport = split[1];
                m_dbname = split[2];
                m_dbuser = split[3];
                m_dbpass = split[4];
                return TRUE;
            }
        }
        return FALSE;
    }

    void SaveConnect()
    {
        std::stringstream pass;
        pass << m_dbhost << std::endl
            << m_dbport << std::endl
            << m_dbname << std::endl
            << m_dbuser << std::endl
            << m_dbpass;

        std::string p = pass.str();
        memset(sCommand2, 0, 0xFFFF);
        strcpy_s(sCommand2, 255, p.c_str());;
        encode(sCommand2, p.length());

        std::ofstream s(SQLFileName, std::ios::binary | std::ios::out | std::ios::trunc);
        if(s.is_open())
        {
            s.write(sCommand2, p.length());
            s.close();
        }
    }

    INT_PTR InitDialog(HWND hWnd)
    {
        CenterWindow(hWnd, NULL);
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT1), m_dbhost.c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT2), m_dbport.c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT3), m_dbname.c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT4), m_dbuser.c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT5), m_dbpass.c_str());
        return 0;
    }

    INT_PTR CommandDialog(HWND hWnd, WPARAM wParam)
    {
        if(wParam == IDOK)
        {
            char ss[256];
            GetWindowText(GetDlgItem(hWnd, IDC_EDIT1), ss, 256);    m_dbhost = ss;
            GetWindowText(GetDlgItem(hWnd, IDC_EDIT2), ss, 256);    m_dbport = ss;
            GetWindowText(GetDlgItem(hWnd, IDC_EDIT3), ss, 256);    m_dbname = ss;
            GetWindowText(GetDlgItem(hWnd, IDC_EDIT4), ss, 256);    m_dbuser = ss;
            GetWindowText(GetDlgItem(hWnd, IDC_EDIT5), ss, 256);    m_dbpass = ss;

            if(conn_spis.Connection())
            {
                EndDialog(hWnd, FALSE);
            }
        }
        if(wParam == IDCANCEL)
        {
            EndDialog(hWnd, FALSE);
            Quit();
        }
        return 0;
    }

    INT_PTR CALLBACK bagSave(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if(message == WM_INITDIALOG)return InitDialog(hWnd);
        if(message == WM_COMMAND) return CommandDialog(hWnd, wParam);
        return (0);
    }

}