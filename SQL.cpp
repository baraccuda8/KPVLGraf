#include "framework.h"
#include "KPVLGraf.h"

#include "SQL.h"

//#include "hard.h"
//#include "SQL.h"

//Соединение с SQL базой
PGConnection conn_spis;

#pragma region Данные для подключения
std::string m_dbhost = "localhost";
std::string m_dbport = "5432";
std::string m_dbname = "";
std::string m_dbuser = "";
std::string m_dbpass = "";
#pragma endregion

namespace CollTag{
    int Id = 0;
    int Name = 0;
    int Type = 0;
    int Arhive = 0;
    int Comment = 0;
    int Content = 0;
    int Coeff = 0;
    int Hist = 0;
    int Format = 0;
    int Idsec = 0;
}



#pragma region Закалочная печи 
std::string PLC210APPL = "|var|PLC210 OPC-UA.Application.";

T_AI_Hmi_210 AI_Hmi_210 = T_AI_Hmi_210(PLC210APPL + "AI_Hmi_210.Hmi.");
T_Hmi210_1 Hmi210_1 = T_Hmi210_1(PLC210APPL + "Hmi210_1.");
T_GenSeqFromHmi GenSeqFromHmi = T_GenSeqFromHmi (PLC210APPL + "GenSeqFromHmi.Data.");
T_GenSeqToHmi GenSeqToHmi = T_GenSeqToHmi(PLC210APPL + "GenSeqToHmi.Data.");
T_Par_Gen Par_Gen = T_Par_Gen(PLC210APPL + "Par_Gen.Par.");

T_PlateData PlateData[SheetMaxPos] = {
    T_PlateData(PLC210APPL + "GenSeqFromHmi.Data.PlateData."),
    T_PlateData(PLC210APPL + "GenSeqToHmi.Data.PlateData_Z1."),
    T_PlateData(PLC210APPL + "GenSeqToHmi.Data.PlateData_Z2."),
    T_PlateData(PLC210APPL + "GenSeqToHmi.Data.PlateData_Z3."),
    T_PlateData(PLC210APPL + "GenSeqToHmi.Data.PlateData_Z4."),
    T_PlateData(PLC210APPL + "GenSeqToHmi.Data.PlateData_Z5."),
    T_PlateData(PLC210APPL + "HMISheetData.Sheet.SheetDataIN.")
};

T_HMISheetData HMISheetData = T_HMISheetData(PLC210APPL + "HMISheetData.Sheet.");
#pragma endregion


#pragma region Печи отпуска 
std::string SPK107APPL = "|var|SPK107 (M01).Application.";

T_Furn Furn_1 = T_Furn(SPK107APPL + "ForBase_RelFurn_1.Data.");
T_Furn Furn_2 = T_Furn(SPK107APPL + "ForBase_RelFurn_2.Data.");

T_CassetteArray CassetteArray = T_CassetteArray(SPK107APPL + "cassetteArray.data.");
#pragma endregion

std::string cp1251_to_utf8(std::string str)
{
    std::string res = "";
    try
    {
        std::wstring ures;

        int result_u, result_c;

        result_u = MultiByteToWideChar(1251, 0, &str[0], -1, 0, 0);
        if(!result_u) return "";

        ures.resize(result_u);
        if(!MultiByteToWideChar(1251, 0, &str[0], -1, &ures[0], result_u)) return "";

        result_c = WideCharToMultiByte(CP_UTF8, 0, &ures[0], -1, 0, 0, 0, 0);
        if(!result_c) return "";

        res.resize(result_c);
        if(!WideCharToMultiByte(CP_UTF8, 0, &ures[0], -1, &res[0], result_c, 0, 0)) return "";
    }CATCH(SQL_LOG, "");
    return res;
}

std::string utf8_to_cp1251(std::string str)
{
    try
    {
        if(!str.length()) return "";

        std::wstring res;

        int convertResult;
        convertResult = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
        if(convertResult <= 0)
            return "";

        res.resize(convertResult);
        if(MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &res[0], (int)res.size()) <= 0)
            return "";

        //convertResult = WideCharToMultiByte(CP_ACP, 0, res.c_str(), (int)res.length(), NULL, 0, NULL, NULL);
        //if(convertResult <= 0)
        //    return "";

        char* ss =  new char[res.length() + 1];
        memset(ss, 0, res.length() + 1);
        if(WideCharToMultiByte(CP_ACP, 0, res.c_str(), (int)res.length(), ss, (int)res.length(), NULL, NULL) <= 0)
            return "";

        str = ss;
        delete[] ss;
    }CATCH(SQL_LOG, "");

    return str;
}

namespace LoginDlg
{
#define SQLFileName "PostgreSQL.dat"

    std::string pKey = "хабраbarracudabarracudaхабра";
    char sCommand[0xFFFF];
    char sCommand2[0xFFFF];


    void encode(char* pText, size_t len)
    {
        for(size_t i = 0; i < len; i++)
            pText[i] = (byte)(pText[i] ^ pKey[i % pKey.length()]);
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

            conn_spis.Connection();
            if(conn_spis.connections)
            {
                SaveConnect();
                EndDialog(hWnd, FALSE);
            }
            else
            {
                MessageBox(NULL, "Ошибка соединения с базой!", "Ошибка!", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);
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
};




void InitCurentTag()
{
    try
    {
#pragma region SELECT FROM tag
        std::string comand = "SELECT id, name, type, arhive, comment, content, coeff, hist, format, idsec FROM tag ORDER BY id;";
        PGresult* res = conn_spis.PGexec(comand);
#pragma endregion

        if(PQresultStatus(res) == PGRES_TUPLES_OK)
        {
            if(!CollTag::Idsec)
            {
                int nFields = PQnfields(res);
                for(int j = 0; j < nFields; j++)
                {
                    std::string l =  utf8_to_cp1251(PQfname(res, j));
                    if(l == "id") CollTag::Id = j;
                    else if(l == "name") CollTag::Name = j;
                    else if(l == "type") CollTag::Type = j;
                    else if(l == "arhive") CollTag::Arhive = j;
                    else if(l == "comment") CollTag::Comment = j;
                    else if(l == "content") CollTag::Content = j;
                    else if(l == "coeff") CollTag::Coeff = j;
                    else if(l == "hist") CollTag::Hist = j;
                    else if(l == "format") CollTag::Format = j;
                    else if(l == "idsec") CollTag::Idsec = j;
                }
            }

            int line = PQntuples(res);
            for(int l = 0; l < line; l++)
            {
                std::string Arhive = PGgetvalue(res, l, CollTag::Arhive);
                if(Arhive == "t") //Если ведется архив
                {
                    std::string Patch = PGgetvalue(res, l, CollTag::Name);

                    if(PlateData[SheetPos0].GetTagTable(res, Patch, l)) continue;
                    if(PlateData[SheetPos1].GetTagTable(res, Patch, l)) continue;
                    if(PlateData[SheetPos2].GetTagTable(res, Patch, l)) continue;
                    if(PlateData[SheetPos3].GetTagTable(res, Patch, l)) continue;
                    if(PlateData[SheetPos4].GetTagTable(res, Patch, l)) continue;
                    if(PlateData[SheetPos5].GetTagTable(res, Patch, l)) continue;
                    if(PlateData[SheetPos6].GetTagTable(res, Patch, l)) continue;
                    if(Par_Gen.GetTagTable(res, Patch, l)) continue;
                    if(Hmi210_1.GetTagTable(res, Patch, l)) continue;
                    if(GenSeqToHmi.GetTagTable(res, Patch, l)) continue;
                    if(GenSeqToHmi.GetTagTable(res, Patch, l)) continue;
                    if(GenSeqFromHmi.GetTagTable(res, Patch, l)) continue;
                    if(AI_Hmi_210.GetTagTable(res, Patch, l)) continue;
                    if(HMISheetData.GetTagTable(res, Patch, l)) continue;

                    if(Furn_1.GetTagTable(res, Patch, l)) continue;
                    if(Furn_2.GetTagTable(res, Patch, l)) continue;

                    if(CassetteArray.GetTagTable(res, Patch, l)) continue;
                    //Для всех остальных тегов, если с ними что то не понятно
                    //else if(Patch.find("PLC210 OPC-UA.Application.HMISheetData.Sheet.") != std::string::npos)
                    //{
                    //    GetTagTable(res, HMISheetData_old, Patch, l, ofs);
                    //}
                    //Для всех остальных тегов, если с ними что то не понятно
                    //else if(Patch.find("SPK107 (M01)") != std::string::npos)
                    //{
                    //    GetTagTable(res, AllTagPeth, Patch, l, ofs);
                    //}
                }
            }
        }
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQL_LOG, res, comand);
        PQclear(res);

#ifdef _DEBUG
#pragma region open ofstream
        std::ofstream ofs("all_tag.csv", std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
        ofs << "id;name;type;arhive;comment;content;coeff;hist;format" << std::endl;
        //Тут надо сделать запись
        if(!ofs.bad())
            ofs.close();
#pragma endregion
#endif

        int tt = 0;
    }CATCH(SQL_LOG, "");
}

void InitTag()
{
    try
    {
        InitCurentTag();

#pragma region SELECT id, content FROM possheet
    //std::string comand = "SELECT id, content FROM possheet"; ///* WHERE name = '" + val->Patch + "'*/;";
    //PGresult* res = conn_kpvl.PGexec(comand);
    //if(PQresultStatus(res) == PGRES_TUPLES_OK)
    //{
    //    int line = PQntuples(res);
    //    for(int l = 0; l < line; l++)
    //        NamePos[conn_kpvl.PGgetvalue(res, l, 0)] = conn_kpvl.PGgetvalue(res, l, 1);
    //}
    //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //    LOG_ERR_SQL(SQLLogger, res, comand);
    //PQclear(res);

    //if(!NamePos.size())
    //{
    //    NamePos = {
    //        {"0", "На входе" },
    //        {"1", "1-я часть печи"},
    //        {"2", "2-я часть печи"},
    //        {"3", "Закалка"},
    //        {"4", "Охлаждение"},
    //        {"5", "Кантовка"},
    //        {"6", "Кантовка"},
    //        {"7", "В касете"},
    //        {"10", "Потерян На входе" },
    //        {"11", "Потерян 1-я часть печи"},
    //        {"12", "Потерян 2-я часть печи"},
    //        {"13", "Потерян Закалке"},
    //        {"14", "Потерян Охлаждении"},
    //        {"15", "Потерян Кантовке"},
    //        {"16", "Потерян Кантовке"},
    //        {"17", "Потерян В касете"},
    //        {"20", "Потерян На входе" },
    //        {"21", "Потерян 1-я часть печи"},
    //        {"22", "Потерян 2-я часть печи"},
    //        {"23", "Потерян Закалке"},
    //        {"24", "Потерян Охлаждении"},
    //        {"25", "Потерян Кантовке"},
    //        {"26", "Потерян Кантовке"},
    //        {"27", "Потерян В касете"},
    //    };

    //    //NamePos["0"] = "На входе";
    //    //NamePos["1"] = "1-я часть печи";
    //    //NamePos["2"] = "2-я часть печи";
    //    //NamePos["3"] = "Закалка";
    //    //NamePos["4"] = "Охлаждение";
    //    //NamePos["5"] = "Кантовка";
    //    //NamePos["6"] = "Кантовка";
    //    //NamePos["7"] = "В касете";
    //    //
    //    //NamePos["10"] = "Потерян На входе";
    //    //NamePos["11"] = "Потерян 1-я часть печи";
    //    //NamePos["12"] = "Потерян 2-я часть печи";
    //    //NamePos["13"] = "Потерян Закалке";
    //    //NamePos["14"] = "Потерян Охлаждении";
    //    //NamePos["15"] = "Потерян Кантовке";
    //    //NamePos["16"] = "Потерян Кантовке";
    //    //NamePos["17"] = "Потерян В касете";

    //    std::stringstream ss;
    //    for(std::map <std::string, std::string>::iterator it = NamePos.begin(); it != NamePos.end(); it++)
    //    {
    //        ss << "INSERT INTO possheet (id, content) VALUES ('" << it->first << "', '" << it->second << "');\n";
    //        NamePos[it->first] = it->second;
    //    }
    //    comand = ss.str();
    //    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //    res = conn_kpvl.PGexec(comand);
    //    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //        LOG_ERR_SQL(SQLLogger, res, comand);
    //    PQclear(res);
    //}
#pragma endregion

#pragma region SELECT id, content FROM EventCassette
    //comand = "SELECT id, content FROM EventCassette"; ///* WHERE name = '" + val->Patch + "'*/;";
    //if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //res = conn_kpvl.PGexec(comand);
    //if(PQresultStatus(res) == PGRES_TUPLES_OK)
    //{
    //    int line = PQntuples(res);
    //    for(int l = 0; l < line; l++)
    //        EventCassette[(evCassete::EV)Stoi(conn_kpvl.PGgetvalue(res, l, 0))] = conn_kpvl.PGgetvalue(res, l, 1).c_str();
    //}
    //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //    LOG_ERR_SQL(SQLLogger, res, comand);
    //PQclear(res);

    //if(!EventCassette.size())
    //{
    //    std::map<int, std::string> eventcassette ={
    //        {evCassete::Nul,  "Неизвестно"},
    //        {evCassete::Fill, "Набирается"},
    //        {evCassete::Wait, "Ожидает"},
    //        {evCassete::Rel, "Отпуск"},
    //        {evCassete::Error, "Авария"},
    //        {evCassete::End, "Конец"},
    //        {evCassete::History, "Из базы"},
    //        {evCassete::Delete, "Удален"},
    //    };

    //    std::stringstream ss;
    //    for(std::map <int, std::string>::iterator it = eventcassette.begin(); it != eventcassette.end(); it++)
    //    {
    //        ss << "INSERT INTO EventCassette (id, content) VALUES (" << it->first << ", '" << it->second << "');\n";
    //        EventCassette[it->first] = it->second;
    //    }

    //    comand = ss.str();
    //    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //    res = conn_kpvl.PGexec(comand);
    //    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //        LOG_ERR_SQL(SQLLogger, res, comand);
    //    PQclear(res);
    //}
#pragma endregion

#pragma region SELECT id, content FROM genseq1
    //comand = "SELECT id, content FROM genseq1"; ///* WHERE name = '" + val->Patch + "'*/;";
    //if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //res = conn_kpvl.PGexec(comand);
    //if(PQresultStatus(res) == PGRES_TUPLES_OK)
    //{
    //    int line = PQntuples(res);
    //    for(int l = 0; l < line; l++)
    //        GenSeq1[atoi(conn_kpvl.PGgetvalue(res, l, 0).c_str())] = conn_kpvl.PGgetvalue(res, l, 1);
    //}
    //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //    LOG_ERR_SQL(SQLLogger, res, comand);
    //PQclear(res);

    //if(!GenSeq1.size())
    //{
    //    GenSeq1 = {
    //        {0, "Отключено"},
    //        {1, "Подготовка"},
    //        {2, "Прогрев"},
    //        {3, "Открыть входную дверь"},
    //        {4, "Загрузка в печь"},
    //        {5, "Закрыть входную дверь"},
    //        {6, "Нагрев листа"},
    //        {7, "Передача на 2 рольганг"},
    //        {8, "Передача на 2-й рольганг печи"},
    //    };

    //    std::stringstream ss;
    //    for(std::map <int, std::string>::iterator it = GenSeq1.begin(); it != GenSeq1.end(); it++)
    //    {
    //        ss << "INSERT INTO genseq1 (id, content) VALUES (" << it->first << ", '" << it->second << "');\n";
    //        GenSeq1[it->first] = it->second;
    //    }

    //    comand = ss.str();
    //    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //    res = conn_kpvl.PGexec(comand);
    //    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //        LOG_ERR_SQL(SQLLogger, res, comand);
    //    PQclear(res);
    //}
#pragma endregion

#pragma region SELECT id, content FROM genseq2
    //comand = "SELECT id, content FROM genseq2"; ///* WHERE name = '" + val->Patch + "'*/;";
    //if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //res = conn_kpvl.PGexec(comand);
    //if(PQresultStatus(res) == PGRES_TUPLES_OK)
    //{
    //    int line = PQntuples(res);
    //    for(int l = 0; l < line; l++)
    //        GenSeq2[atoi(conn_kpvl.PGgetvalue(res, l, 0).c_str())] = conn_kpvl.PGgetvalue(res, l, 1);
    //}
    //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //    LOG_ERR_SQL(SQLLogger, res, comand);
    //PQclear(res);

    //if(!GenSeq2.size())
    //{
    //    GenSeq2 = {
    //        {0, "Отключено"},
    //        {1, "Подготовка"},
    //        {2, "Прогрев"},
    //        {3, "Прием заготовки с 1-го рольганга печи"},
    //        {4, "Осциляция. Нагрев Листа"},
    //        {5, "Открыть выходную дверь"},
    //        {6, "Выдача в линию закалки"},
    //        {7, "Закрыть выходную дверь"},
    //    };

    //    std::stringstream ss;
    //    for(std::map <int, std::string>::iterator it = GenSeq2.begin(); it != GenSeq2.end(); it++)
    //    {
    //        ss << "INSERT INTO genseq2 (id, content) VALUES (" << it->first << ", '" << it->second << "');\n";
    //        GenSeq2[it->first] = it->second;
    //    }

    //    comand = ss.str();
    //    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //    res = conn_kpvl.PGexec(comand);
    //    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //        LOG_ERR_SQL(SQLLogger, res, comand);
    //    PQclear(res);
    //}
#pragma endregion

#pragma region SELECT id, content FROM genseq3
    //comand = "SELECT id, content FROM genseq3"; ///* WHERE name = '" + val->Patch + "'*/;";
    //if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //res = conn_kpvl.PGexec(comand);
    //if(PQresultStatus(res) == PGRES_TUPLES_OK)
    //{
    //    int line = PQntuples(res);
    //    for(int l = 0; l < line; l++)
    //        GenSeq3[atoi(conn_kpvl.PGgetvalue(res, l, 0).c_str())] = conn_kpvl.PGgetvalue(res, l, 1);
    //}
    //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //    LOG_ERR_SQL(SQLLogger, res, comand);
    //PQclear(res);

    //if(!GenSeq3.size())
    //{
    //    GenSeq3 = {
    //        {0, "Отключено"},
    //        {1, "Ожидание листа"},
    //        {2, "Осциляция. Охл.листа."},
    //        {3, "Выдача заготовки"},
    //        {4, "Окончание цикла обработки"},
    //    };

    //    std::stringstream ss;
    //    for(std::map <int, std::string>::iterator it = GenSeq3.begin(); it != GenSeq3.end(); it++)
    //    {
    //        ss << "INSERT INTO genseq3 (id, content) VALUES (" << it->first << ", '" << it->second << "');\n";
    //        GenSeq3[it->first] = it->second;
    //    }

    //    comand = ss.str();
    //    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //    res = conn_kpvl.PGexec(comand);
    //    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //        LOG_ERR_SQL(SQLLogger, res, comand);
    //    PQclear(res);
    //}
#pragma endregion
    }CATCH(SQL_LOG, "");
}

#define SQL_LOG_REM (std::string(SQL_LOG) + ".log").c_str()
bool InitSQL()
{
    try
    {
        remove(SQL_LOG_REM);
        if(!LoginDlg::LoadConnect())
        {
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, LoginDlg::bagSave);
            if(!conn_spis.connections && !conn_spis.Connection())
                throw std::exception("Error SQL conn_spis connection");
        }
        else
        {
            conn_spis.Connection();
            if(!conn_spis.connections && !conn_spis.Connection())
                throw std::exception("Error SQL conn_spis connection");
        }
        InitTag();
    }
    catch(std::exception& exc)
    {
        WinErrorExit(NULL, exc.what());
    }
    catch(...)
    {
        WinErrorExit(NULL, "Unknown error.");
    }


    return conn_spis.connections;
}

