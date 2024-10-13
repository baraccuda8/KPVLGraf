#include "framework.h"
#include "KPVLGraf.h"

#include "SQL.h"
#include "LoginDlg.h"

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


HANDLE hSqlTag = NULL;
std::map<int, Value*>AllTag;

std::map<std::string, std::string> NamePos;
std::map<int, std::string> EventCassette;

std::map<int, std::string> GenSeq1;
std::map<int, std::string> GenSeq2;
std::map<int, std::string> GenSeq3;


#pragma region Закалочная печи 
std::string PLC210APPL = "|var|PLC210 OPC-UA.Application.";

T_AI_Hmi_210 AI_Hmi_210 = T_AI_Hmi_210(PLC210APPL + "AI_Hmi_210.Hmi.");
T_Hmi210_1 Hmi210_1 = T_Hmi210_1(PLC210APPL + "Hmi210_1.");
T_GenSeqFromHmi GenSeqFromHmi = T_GenSeqFromHmi (PLC210APPL + "GenSeqFromHmi.Data.");
T_GenSeqToHmi GenSeqToHmi = T_GenSeqToHmi(PLC210APPL + "GenSeqToHmi.Data.");
T_Par_Gen Par_Gen = T_Par_Gen(PLC210APPL + "Par_Gen.Par.");
T_HMISheetData HMISheetData = T_HMISheetData(PLC210APPL + "HMISheetData.Sheet.");

T_PlateData PlateData[SheetMaxPos] = {
    T_PlateData(PLC210APPL + "GenSeqFromHmi.Data.PlateData."),
    T_PlateData(PLC210APPL + "GenSeqToHmi.Data.PlateData_Z1."),
    T_PlateData(PLC210APPL + "GenSeqToHmi.Data.PlateData_Z2."),
    T_PlateData(PLC210APPL + "GenSeqToHmi.Data.PlateData_Z3."),
    T_PlateData(PLC210APPL + "GenSeqToHmi.Data.PlateData_Z4."),
    T_PlateData(PLC210APPL + "GenSeqToHmi.Data.PlateData_Z5."),
    T_PlateData(PLC210APPL + "HMISheetData.Sheet.SheetDataIN.")
};

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
    }CATCH(SQLLogger, "");
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
    }CATCH(SQLLogger, "");

    return str;
}


void InitCurentTag()
{
    try
    {
        std::string comand = "SELECT id, name, type, arhive, comment, content, coeff, hist, format, idsec FROM tag ORDER BY id;";
        PGresult* res = conn_spis.PGexec(comand);

        if(PQresultStatus(res) == PGRES_TUPLES_OK)
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

            int line = PQntuples(res);
            for(int l = 0; l < line; l++)
            {
                std::string Arhive = PGgetvalue(res, l, CollTag::Arhive);
                if(Arhive == "t") //Если ведется архив
                {
                    std::string patch = PGgetvalue(res, l, CollTag::Name);

                    if(PlateData[SheetPos0].GetTagTable(res, l, patch)) continue;
                    if(PlateData[SheetPos1].GetTagTable(res, l, patch)) continue;
                    if(PlateData[SheetPos2].GetTagTable(res, l, patch)) continue;
                    if(PlateData[SheetPos3].GetTagTable(res, l, patch)) continue;
                    if(PlateData[SheetPos4].GetTagTable(res, l, patch)) continue;
                    if(PlateData[SheetPos5].GetTagTable(res, l, patch)) continue;
                    if(PlateData[SheetPos6].GetTagTable(res, l, patch)) continue;
                    if(Par_Gen.GetTagTable(res, l, patch)) continue;
                    if(Hmi210_1.GetTagTable(res, l, patch)) continue;
                    if(GenSeqToHmi.GetTagTable(res, l, patch)) continue;
                    if(GenSeqToHmi.GetTagTable(res, l, patch)) continue;
                    if(GenSeqFromHmi.GetTagTable(res, l, patch)) continue;
                    if(AI_Hmi_210.GetTagTable(res, l, patch)) continue;
                    if(HMISheetData.GetTagTable(res, l, patch)) continue;

                    if(Furn_1.GetTagTable(res, l, patch)) continue;
                    if(Furn_2.GetTagTable(res, l, patch)) continue;

                    if(CassetteArray.GetTagTable(res, l, patch)) continue;
                    
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
        else if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);

#pragma region open ofstream
//        std::ofstream ofs("all_tag.csv", std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
//        ofs << "id;name;type;arhive;comment;content;coeff;hist;format" << std::endl;
//        //Тут надо сделать запись
//        if(!ofs.bad())
//            ofs.close();
#pragma endregion

        int tt = 0;
    }CATCH(SQLLogger, "InitCurentTag");
}

DWORD WINAPI CurentValue(LPVOID)
{
    try
    {
        while(isRun)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            std::string comand = "SELECT id, content FROM tag ORDER BY id;";
            PGresult* res = conn_spis.PGexec(comand);

            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                int line = PQntuples(res);
                for(int l = 0; l < line; l++)
                {
                    int id = Stoi(PGgetvalue(res, l, 0));

                    for(auto& a : AllTag)
                        if(a.second->GetCurentValue(res, l, id)) continue;

//                if(PlateData[SheetPos0].GetCurentValue(res, l, id)) continue;
//                if(PlateData[SheetPos1].GetCurentValue(res, l, id)) continue;
//                if(PlateData[SheetPos2].GetCurentValue(res, l, id)) continue;
//                if(PlateData[SheetPos3].GetCurentValue(res, l, id)) continue;
//                if(PlateData[SheetPos4].GetCurentValue(res, l, id)) continue;
//                if(PlateData[SheetPos5].GetCurentValue(res, l, id)) continue;
//                if(PlateData[SheetPos6].GetCurentValue(res, l, id)) continue;
//                if(Par_Gen.GetCurentValue(res, l, id)) continue;
//                if(Hmi210_1.GetCurentValue(res, l, id)) continue;
//                if(GenSeqToHmi.GetCurentValue(res, l, id)) continue;
//                if(GenSeqToHmi.GetCurentValue(res, l, id)) continue;
//                if(GenSeqFromHmi.GetCurentValue(res, l, id)) continue;
//                if(AI_Hmi_210.GetCurentValue(res, l, id)) continue;
//                if(HMISheetData.GetCurentValue(res, l, id)) continue;
//
//                if(Furn_1.GetCurentValue(res, l, id)) continue;
//                if(Furn_2.GetCurentValue(res, l, id)) continue;
//
//                if(CassetteArray.GetCurentValue(res, l, id)) continue;
//
                }
            }
            else if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);
            std::string s = "Furn_1: " + Furn_1.ActTimeTotal.Content + "  Furn_2: " + Furn_2.ActTimeTotal.Content;
            SetWindowText(GlobalhWnd, s.c_str());
            
        }
        int tt = 0;
    }CATCH(SQLLogger, "InitCurentTag");
    return 0;
}

void InitTag()
{
    InitCurentTag();

#pragma region SELECT id, content FROM possheet
    try
    {
        std::string comand = "SELECT id, content FROM possheet";
        PGresult* res = conn_spis.PGexec(comand);
        if(PQresultStatus(res) == PGRES_TUPLES_OK)
        {
            int line = conn_spis.PQntuples(res);
            for(int l = 0; l < line; l++)
                NamePos[conn_spis.PGgetvalue(res, l, 0)] = conn_spis.PGgetvalue(res, l, 1);
        }
        else if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);

    }CATCH(SQLLogger, "possheet");
#pragma endregion

#pragma region SELECT id, content FROM EventCassette
    try
    {
        std::string comand = "SELECT id, content FROM EventCassette";
        PGresult* res = conn_spis.PGexec(comand);
        if(PQresultStatus(res) == PGRES_TUPLES_OK)
        {
            int line = PQntuples(res);
            for(int l = 0; l < line; l++)
                EventCassette[(evCassete::EV)Stoi(conn_spis.PGgetvalue(res, l, 0))] = conn_spis.PGgetvalue(res, l, 1).c_str();
        }
        else if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);

    }CATCH(SQLLogger, "EventCassette");

#pragma endregion

#pragma region SELECT id, content FROM genseq1
    try
    {
        std::string comand = "SELECT id, content FROM genseq1";
        PGresult* res = conn_spis.PGexec(comand);
        if(PQresultStatus(res) == PGRES_TUPLES_OK)
        {
            int line = PQntuples(res);
            for(int l = 0; l < line; l++)
                GenSeq1[atoi(conn_spis.PGgetvalue(res, l, 0).c_str())] = conn_spis.PGgetvalue(res, l, 1);
        }
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);
    }CATCH(SQLLogger, "genseq1");
#pragma endregion

#pragma region SELECT id, content FROM genseq2
    try
    {
        std::string comand = "SELECT id, content FROM genseq2"; ///* WHERE name = '" + val->Patch + "'*/;";
        PGresult* res = conn_spis.PGexec(comand);
        if(PQresultStatus(res) == PGRES_TUPLES_OK)
        {
            int line = PQntuples(res);
            for(int l = 0; l < line; l++)
                GenSeq2[atoi(conn_spis.PGgetvalue(res, l, 0).c_str())] = conn_spis.PGgetvalue(res, l, 1);
        }
        else if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);
    }CATCH(SQLLogger, "genseq2");
#pragma endregion

#pragma region SELECT id, content FROM genseq3
    try
    {
        std::string comand = "SELECT id, content FROM genseq3"; ///* WHERE name = '" + val->Patch + "'*/;";
        PGresult* res = conn_spis.PGexec(comand);
        if(PQresultStatus(res) == PGRES_TUPLES_OK)
        {
            int line = PQntuples(res);
            for(int l = 0; l < line; l++)
                GenSeq3[atoi(conn_spis.PGgetvalue(res, l, 0).c_str())] = conn_spis.PGgetvalue(res, l, 1);
        }
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);
    }CATCH(SQLLogger, "genseq3");

    //    comand = ss.str();
    //    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //    res = conn_kpvl.PGexec(comand);
    //    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //        LOG_ERR_SQL(SQLLogger, res, comand);
    //    PQclear(res);
    //}
#pragma endregion
    
    int ta = 0;
}

#define SQL_LOG_REM (std::string(SQLLogger) + ".log").c_str()
bool InitSQL()
{
    try
    {
        remove(SQL_LOG_REM);
        if(!LoginDlg::LoadConnect())
        {
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, LoginDlg::bagSave);
            if(!conn_spis.Connection())
                throw std::exception("Error SQL conn_spis connection");
        }
        else
        {
            conn_spis.Connection();
            if(!conn_spis.connections && !conn_spis.Connection())
                throw std::exception("Error SQL conn_spis connection");
        }
        InitTag();
        hSqlTag = CreateThread(0, 0, CurentValue, (LPVOID)0, 0, 0);
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


void StopSql()
{
    WaitCloseTheread(hSqlTag);
}