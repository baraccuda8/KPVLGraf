#include "framework.h"
#include "KPVLGraf.h"
#include "Value.h"

#include "SQL.h"

//#include "hard.h"
//#include "SQL.h"

//Соединение с SQL базой
PGConnection conn_spis;


std::string m_dbhost = "192.168.9.63";
std::string m_dbport = "5432";
std::string m_dbname = "kpvl";
std::string m_dbuser = "user";
std::string m_dbpass = "TutonHamon8*";


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


VALUE AllTagKpvl;
VALUE AllTagPeth;

VALUE HMISheetData;
//VALUE AI_Hmi_210;
struct T_AI_Hmi_210{
    //REAL Давление воды закалка коллектор низ
    Value LaminPressBot = Value("|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.LaminPressBot.AI_eu");

    //REAL Давление воды закалка коллектор верх
    Value LaminPressTop = Value("|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.LaminPressTop.AI_eu");

    //REAL Температура воды в поддоне
    Value LAM_TE1 = Value("|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.LAM_TE1.AI_eu");

    //REAL Температура воды в баке
    Value Za_PT3 = Value("|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.Za_PT3.AI_eu");

    //REAL Температура воды в баке
    Value Za_TE3 = Value("|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.Za_TE3.AI_eu");

    //REAL Пирометр
    Value Za_TE4 = Value("|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.Za_TE4.AI_eu");
}AI_Hmi_210;

struct T_GenSeqFromHmi{
    //REAL Уставки температуры в печи
    Value TempSet1 = Value("|var|PLC210 OPC-UA.Application.GenSeqFromHmi.Data.TempSet1");
}GenSeqFromHmi;

struct T_GenSeqToHmi{
    //REAL Сколько прошло времени охлаждения
    Value CoolTimeAct = Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.CoolTimeAct");

    //REAL время нагрева в зоне 1
    Value HeatTime_Z1 = Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.HeatTime_Z1");

    //REAL время нагрева в зоне 2
    Value HeatTime_Z2 = Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.HeatTime_Z2");

    //INT Номер шага последовательности загрузки в печь
    Value Seq_1_StateNo = Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.Seq_1_StateNo");

    //INT Номер шага последовательности выгрузки в печи
    Value Seq_2_StateNo = Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.Seq_2_StateNo");

    //INT Номер шага последовательности обработки в ламинарке
    Value Seq_3_StateNo = Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.Seq_3_StateNo");

}GenSeqToHmi;

//VALUE GenSeqToHmi;
//VALUE Par_Gen;

struct T_Par_Gen {

    //REAL Уставка давления для запуска комперссора
    Value PresToStartComp = Value("|var|PLC210 OPC-UA.Application.Par_Gen.Par.PresToStartComp");

    //REAL Время сигнализации окончания нагрева, мин
    Value TimeForPlateHeat = Value("|var|PLC210 OPC-UA.Application.Par_Gen.Par.TimeForPlateHeat");

    //REAL Скорость выгрузки
    Value UnloadSpeed = Value("|var|PLC210 OPC-UA.Application.Par_Gen.Par.UnloadSpeed");
}Par_Gen;


typedef struct T_PlateData{
    //Char Код марки
    Value AlloyCodeText;

    //DINT номер плавки
    Value Melt;

    //DINT Пачка
    Value Pack;

    //DINT номер партии
    Value PartNo;

    //DINT номер листа
    Value Sheet;

    //DINT Номер сляба
    Value Slab;

    //DINT подномер листа
    Value SubSheet;

    //Char код толщины. Зависит от марки стали.
    Value ThiknessText;
};

T_PlateData PlateData_Z0 ={
    Value("|var|PLC210 OPC-UA.Application.GenSeqFromHmi.Data.PlateData.AlloyCodeText.sText"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqFromHmi.Data.PlateData.Melt"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqFromHmi.Data.PlateData.Pack"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqFromHmi.Data.PlateData.PartNo"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqFromHmi.Data.PlateData.Sheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqFromHmi.Data.PlateData.Slab"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqFromHmi.Data.PlateData.SubSheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqFromHmi.Data.PlateData.ThiknessText.sText"),
};
T_PlateData PlateData_Z1 ={
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.AlloyCodeText.sText"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.Melt"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.Pack"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.PartNo"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.Sheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.Slab"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.SubSheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.ThiknessText.sText"),
};
T_PlateData PlateData_Z2 ={
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.AlloyCodeText.sText"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.Melt"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.Pack"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.PartNo"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.Sheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.Slab"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.SubSheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.ThiknessText.sText"),
};
T_PlateData PlateData_Z3 ={
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.AlloyCodeText.sText"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.Melt"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.Pack"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.PartNo"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.Sheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.Slab"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.SubSheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.ThiknessText.sText"),
};
T_PlateData PlateData_Z4 ={
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.AlloyCodeText.sText"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.Melt"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.Pack"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.PartNo"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.Sheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.Slab"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.SubSheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.ThiknessText.sText"),
};
T_PlateData PlateData_Z5 ={
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.AlloyCodeText.sText"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.Melt"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.Pack"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.PartNo"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.Sheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.Slab"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.SubSheet"),
    Value("|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.ThiknessText.sText"),
};
T_PlateData PlateData_Z6 ={
    Value("|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.AlloyCodeText.sText"),
    Value("|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.Melt"),
    Value("|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.Pack"),
    Value("|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.PartNo"),
    Value("|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.Sheet"),
    Value("|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.Slab"),
    Value("|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.SubSheet"),
    Value("|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.ThiknessText.sText"),
};

//VALUE PlateData_Z1;
//VALUE PlateData_Z2;
//VALUE PlateData_Z3;
//VALUE PlateData_Z4;
//VALUE PlateData_Z5;
//VALUE PlateData_Z6;


struct T_Hmi210_1{
    //REAL Температура в зоне 1.1
    Value Htr1_1 = Value ("|var|PLC210 OPC-UA.Application.Hmi210_1.Htr_1.ToHmi.TAct");

    //REAL Температура в зоне 1.2
    Value Htr1_2 = Value ("|var|PLC210 OPC-UA.Application.Hmi210_1.Htr_2.ToHmi.TAct");

    //REAL Температура в зоне 1.3
    Value Htr1_3 = Value ("|var|PLC210 OPC-UA.Application.Hmi210_1.Htr_3.ToHmi.TAct");

    //REAL Температура в зоне 1.4
    Value Htr1_4 = Value ("|var|PLC210 OPC-UA.Application.Hmi210_1.Htr_4.ToHmi.TAct");

    //REAL Температура в зоне 2.1
    Value Htr2_1 = Value ("|var|PLC210 OPC-UA.Application.Hmi210_1.Htr2_1.ToHmi.TAct");

    //REAL Температура в зоне 2.2
    Value Htr2_2 = Value ("|var|PLC210 OPC-UA.Application.Hmi210_1.Htr2_2.ToHmi.TAct");

    //REAL Температура в зоне 2.3
    Value Htr2_3 = Value ("|var|PLC210 OPC-UA.Application.Hmi210_1.Htr2_3.ToHmi.TAct");

    //REAL Температура в зоне 2.4
    Value Htr2_4 = Value ("|var|PLC210 OPC-UA.Application.Hmi210_1.Htr2_4.ToHmi.TAct");
}Hmi210_1;

VALUE Furn_1;
VALUE Furn_2;




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
                //SaveConnect();
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
};


void SetValue(Value& val)
{
    try
    {
             if(val.Type == VariantType::BOOLEAN)   val.Val = (bool)(val.content == "true");      // atoi_t(bool, atoi, value);
        else if(val.Type == VariantType::SBYTE)     val.Val = int8_t((std::stoi(val.content)) / (int8_t)val.coeff);
        else if(val.Type == VariantType::BYTE)      val.Val = uint8_t((std::stoi(val.content)) / (uint8_t)val.coeff);
        else if(val.Type == VariantType::INT16)     val.Val = int16_t((std::stoi(val.content)) / (int16_t)val.coeff);
        else if(val.Type == VariantType::UINT16)    val.Val = uint16_t((std::stoi(val.content)) / (uint16_t)val.coeff);
        else if(val.Type == VariantType::INT32)     val.Val = int32_t((std::stoi(val.content)) / (int32_t)val.coeff);
        else if(val.Type == VariantType::UINT32)    val.Val = uint32_t((std::stoll(val.content)) / (uint32_t)val.coeff);
        else if(val.Type == VariantType::INT64)     val.Val = int64_t((std::stoll(val.content)) / (int64_t)val.coeff);
        else if(val.Type == VariantType::UINT64)    val.Val = uint64_t((std::stod(val.content)) / (uint64_t)val.coeff);
        else if(val.Type == VariantType::FLOAT)     val.Val = float((std::stof(val.content)) / (float)val.coeff);
        else if(val.Type == VariantType::DOUBLE)    val.Val = double((std::stod(val.content)) / (double)val.coeff);
        else if(val.Type == VariantType::STRING)    val.Val =  cp1251_to_utf8(val.content); // utf8_to_cp1251(value); //cp1251_to_utf8
        val.GetString();
    }CATCH(SQL_LOG, "");
}

std::string GetType(VariantType type, std::string value)
{
    try
    {
        if(type == VariantType::BOOLEAN)   return "(bool)" + value;
        else if(type == VariantType::SBYTE)     return "(int8_t)" + value;
        else if(type == VariantType::BYTE)      return "(uint8_t)" + value;
        else if(type == VariantType::INT16)     return "(int16_t)" + value;
        else if(type == VariantType::UINT16)    return "(uint16_t)" + value;
        else if(type == VariantType::INT32)     return "(int32_t)" + value;
        else if(type == VariantType::UINT32)    return "(uint32_t)" + value;
        else if(type == VariantType::INT64)     return "(int64_t)" + value;
        else if(type == VariantType::UINT64)    return "(uint64_t)" + value;
        else if(type == VariantType::FLOAT)     return "(float)" + value;
        else if(type == VariantType::DOUBLE)    return "(double)" + value;
        else if(type == VariantType::STRING)    return "std::string(\"" + value + "\")";
    }CATCH(SQL_LOG, "");
    return "";
}

void GetTagTable(VALUE& All, std::string Patch, PGresult* res, int l, std::ofstream& ofs)
{
    try
    {
        Value val;
        val.Patch = Patch;
        val.ID = atol(conn_spis.PGgetvalue(res, l, CollTag::Id).c_str());
        val.Arhive = conn_spis.PGgetvalue(res, l, CollTag::Arhive) == "t";
        val.Comment = conn_spis.PGgetvalue(res, l, CollTag::Comment);
        val.coeff = static_cast<float>(atof(conn_spis.PGgetvalue(res, l, CollTag::Coeff).c_str()));
        val.hist = static_cast<float>(atof(conn_spis.PGgetvalue(res, l, CollTag::Hist).c_str()));
        val.format = conn_spis.PGgetvalue(res, l, CollTag::Format);

        val.Type =  static_cast<VariantType>(Stoi(conn_spis.PGgetvalue(res, l, CollTag::Type)));
        val.content = conn_spis.PGgetvalue(res, l, CollTag::Content);
        SetValue(val);

        All.push_back(val);

        //boost::replace_all(value, ".", ",");
        if(!ofs.bad())
        {
            ofs << val.ID << ";";
            ofs << "\"\"\"" << val.Patch << "\"\"\";";
            ofs << (int)val.GetType() << ";";
            ofs << val.Arhive << ";";
            ofs << "\"\"\"" << val.Comment << "\"\"\";";
            ofs << GetType(val.Type, val.GetString()) << ";";
            ofs << val.coeff << ";";
            ofs << val.hist << ";";
            ofs << "\"\"\"" << val.format << "\"\"\";";
            ofs << std::endl;
        }
    }CATCH(SQL_LOG, "");
}


void GetTagTable(Value& val, /*std::string Patch, */PGresult* res, int l, std::ofstream& ofs)
{
    try
    {
        //Value val;
        //val.Patch = Patch;
        val.ID = atol(conn_spis.PGgetvalue(res, l, CollTag::Id).c_str());
        val.Arhive = conn_spis.PGgetvalue(res, l, CollTag::Arhive) == "t";
        val.Comment = conn_spis.PGgetvalue(res, l, CollTag::Comment);
        val.coeff = static_cast<float>(atof(conn_spis.PGgetvalue(res, l, CollTag::Coeff).c_str()));
        val.hist = static_cast<float>(atof(conn_spis.PGgetvalue(res, l, CollTag::Hist).c_str()));
        val.format = conn_spis.PGgetvalue(res, l, CollTag::Format);

        val.Type =  static_cast<VariantType>(Stoi(conn_spis.PGgetvalue(res, l, CollTag::Type)));
        val.content = conn_spis.PGgetvalue(res, l, CollTag::Content);
        SetValue(val);

        //All.push_back(val);

        //boost::replace_all(value, ".", ",");
        if(!ofs.bad())
        {
            ofs << val.ID << ";";
            ofs << "\"\"\"" << val.Patch << "\"\"\";";
            ofs << (int)val.GetType() << ";";
            ofs << val.Arhive << ";";
            ofs << "\"\"\"" << val.Comment << "\"\"\";";
            ofs << GetType(val.Type, val.GetString()) << ";";
            ofs << val.coeff << ";";
            ofs << val.hist << ";";
            ofs << "\"\"\"" << val.format << "\"\"\";";
            ofs << std::endl;
        }
    }CATCH(SQL_LOG, "");
}

void InitCurentTag()
{
    try
    {
#pragma region SELECT id, name, type, arhive, comment, content, coeff, hist, format, idsec FROM tag ORDER BY id
        std::ofstream ofs("all_tag.csv", std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
        ofs << "id;name;type;arhive;comment;content;coeff;hist;format" << std::endl;
        std::string comand = "SELECT id, name, type, arhive, comment, content, coeff, hist, format, idsec FROM tag ORDER BY id;";
        PGresult* res = conn_spis.PGexec(comand);
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
                std::string Patch = conn_spis.PGgetvalue(res, l, CollTag::Name);
                if(Patch.find("PLC210") != std::string::npos)
                {
                    if(Patch == PlateData_Z0.AlloyCodeText.Patch)
                        GetTagTable(PlateData_Z0.AlloyCodeText, res, l, ofs);
                    else if(Patch == PlateData_Z0.Melt.Patch)
                        GetTagTable(PlateData_Z0.Melt, res, l, ofs);
                    else if(Patch == PlateData_Z0.Pack.Patch)
                        GetTagTable(PlateData_Z0.Pack, res, l, ofs);
                    else if(Patch == PlateData_Z0.PartNo.Patch)
                        GetTagTable(PlateData_Z0.PartNo, res, l, ofs);
                    else if(Patch == PlateData_Z0.Sheet.Patch)
                        GetTagTable(PlateData_Z0.Sheet, res, l, ofs);
                    else if(Patch == PlateData_Z0.Slab.Patch)
                        GetTagTable(PlateData_Z0.Slab, res, l, ofs);
                    else if(Patch == PlateData_Z0.SubSheet.Patch)
                        GetTagTable(PlateData_Z0.SubSheet, res, l, ofs);
                    else if(Patch == PlateData_Z0.ThiknessText.Patch)
                        GetTagTable(PlateData_Z0.ThiknessText, res, l, ofs);

                    else if(Patch == PlateData_Z1.AlloyCodeText.Patch)
                        GetTagTable(PlateData_Z1.AlloyCodeText, res, l, ofs);
                    else if(Patch == PlateData_Z1.Melt.Patch)
                        GetTagTable(PlateData_Z1.Melt, res, l, ofs);
                    else if(Patch == PlateData_Z1.Pack.Patch)
                        GetTagTable(PlateData_Z1.Pack, res, l, ofs);
                    else if(Patch == PlateData_Z1.PartNo.Patch)
                        GetTagTable(PlateData_Z1.PartNo, res, l, ofs);
                    else if(Patch == PlateData_Z1.Sheet.Patch)
                        GetTagTable(PlateData_Z1.Sheet, res, l, ofs);
                    else if(Patch == PlateData_Z1.Slab.Patch)
                        GetTagTable(PlateData_Z1.Slab, res, l, ofs);
                    else if(Patch == PlateData_Z1.SubSheet.Patch)
                        GetTagTable(PlateData_Z1.SubSheet, res, l, ofs);
                    else if(Patch == PlateData_Z1.ThiknessText.Patch)
                        GetTagTable(PlateData_Z1.ThiknessText, res, l, ofs);

                    else if(Patch == PlateData_Z2.AlloyCodeText.Patch)
                        GetTagTable(PlateData_Z2.AlloyCodeText, res, l, ofs);
                    else if(Patch == PlateData_Z2.Melt.Patch)
                        GetTagTable(PlateData_Z2.Melt, res, l, ofs);
                    else if(Patch == PlateData_Z2.Pack.Patch)
                        GetTagTable(PlateData_Z2.Pack, res, l, ofs);
                    else if(Patch == PlateData_Z2.PartNo.Patch)
                        GetTagTable(PlateData_Z2.PartNo, res, l, ofs);
                    else if(Patch == PlateData_Z2.Sheet.Patch)
                        GetTagTable(PlateData_Z2.Sheet, res, l, ofs);
                    else if(Patch == PlateData_Z2.Slab.Patch)
                        GetTagTable(PlateData_Z2.Slab, res, l, ofs);
                    else if(Patch == PlateData_Z2.SubSheet.Patch)
                        GetTagTable(PlateData_Z2.SubSheet, res, l, ofs);
                    else if(Patch == PlateData_Z2.ThiknessText.Patch)
                        GetTagTable(PlateData_Z2.ThiknessText, res, l, ofs);

                    else if(Patch == PlateData_Z3.AlloyCodeText.Patch)
                        GetTagTable(PlateData_Z3.AlloyCodeText, res, l, ofs);
                    else if(Patch == PlateData_Z3.Melt.Patch)
                        GetTagTable(PlateData_Z3.Melt, res, l, ofs);
                    else if(Patch == PlateData_Z3.Pack.Patch)
                        GetTagTable(PlateData_Z3.Pack, res, l, ofs);
                    else if(Patch == PlateData_Z3.PartNo.Patch)
                        GetTagTable(PlateData_Z3.PartNo, res, l, ofs);
                    else if(Patch == PlateData_Z3.Sheet.Patch)
                        GetTagTable(PlateData_Z3.Sheet, res, l, ofs);
                    else if(Patch == PlateData_Z3.Slab.Patch)
                        GetTagTable(PlateData_Z3.Slab, res, l, ofs);
                    else if(Patch == PlateData_Z3.SubSheet.Patch)
                        GetTagTable(PlateData_Z3.SubSheet, res, l, ofs);
                    else if(Patch == PlateData_Z3.ThiknessText.Patch)
                        GetTagTable(PlateData_Z3.ThiknessText, res, l, ofs);

                    else if(Patch == PlateData_Z4.AlloyCodeText.Patch)
                        GetTagTable(PlateData_Z4.AlloyCodeText, res, l, ofs);
                    else if(Patch == PlateData_Z4.Melt.Patch)
                        GetTagTable(PlateData_Z4.Melt, res, l, ofs);
                    else if(Patch == PlateData_Z4.Pack.Patch)
                        GetTagTable(PlateData_Z4.Pack, res, l, ofs);
                    else if(Patch == PlateData_Z4.PartNo.Patch)
                        GetTagTable(PlateData_Z4.PartNo, res, l, ofs);
                    else if(Patch == PlateData_Z4.Sheet.Patch)
                        GetTagTable(PlateData_Z4.Sheet, res, l, ofs);
                    else if(Patch == PlateData_Z4.Slab.Patch)
                        GetTagTable(PlateData_Z4.Slab, res, l, ofs);
                    else if(Patch == PlateData_Z4.SubSheet.Patch)
                        GetTagTable(PlateData_Z4.SubSheet, res, l, ofs);
                    else if(Patch == PlateData_Z4.ThiknessText.Patch)
                        GetTagTable(PlateData_Z4.ThiknessText, res, l, ofs);

                    else if(Patch == PlateData_Z5.AlloyCodeText.Patch)
                        GetTagTable(PlateData_Z5.AlloyCodeText, res, l, ofs);
                    else if(Patch == PlateData_Z5.Melt.Patch)
                        GetTagTable(PlateData_Z5.Melt, res, l, ofs);
                    else if(Patch == PlateData_Z5.Pack.Patch)
                        GetTagTable(PlateData_Z5.Pack, res, l, ofs);
                    else if(Patch == PlateData_Z5.PartNo.Patch)
                        GetTagTable(PlateData_Z5.PartNo, res, l, ofs);
                    else if(Patch == PlateData_Z5.Sheet.Patch)
                        GetTagTable(PlateData_Z5.Sheet, res, l, ofs);
                    else if(Patch == PlateData_Z5.Slab.Patch)
                        GetTagTable(PlateData_Z5.Slab, res, l, ofs);
                    else if(Patch == PlateData_Z5.SubSheet.Patch)
                        GetTagTable(PlateData_Z5.SubSheet, res, l, ofs);
                    else if(Patch == PlateData_Z5.ThiknessText.Patch)
                        GetTagTable(PlateData_Z5.ThiknessText, res, l, ofs);

                    else if(Patch == PlateData_Z6.AlloyCodeText.Patch)
                        GetTagTable(PlateData_Z6.AlloyCodeText, res, l, ofs);
                    else if(Patch == PlateData_Z6.Melt.Patch)
                        GetTagTable(PlateData_Z6.Melt, res, l, ofs);
                    else if(Patch == PlateData_Z6.Pack.Patch)
                        GetTagTable(PlateData_Z6.Pack, res, l, ofs);
                    else if(Patch == PlateData_Z6.PartNo.Patch)
                        GetTagTable(PlateData_Z6.PartNo, res, l, ofs);
                    else if(Patch == PlateData_Z6.Sheet.Patch)
                        GetTagTable(PlateData_Z6.Sheet, res, l, ofs);
                    else if(Patch == PlateData_Z6.Slab.Patch)
                        GetTagTable(PlateData_Z6.Slab, res, l, ofs);
                    else if(Patch == PlateData_Z6.SubSheet.Patch)
                        GetTagTable(PlateData_Z6.SubSheet, res, l, ofs);
                    else if(Patch == PlateData_Z6.ThiknessText.Patch)
                        GetTagTable(PlateData_Z6.ThiknessText, res, l, ofs);

                    else if(Patch == Par_Gen.PresToStartComp.Patch)
                        GetTagTable(Par_Gen.PresToStartComp, res, l, ofs);
                    else if(Patch == Par_Gen.TimeForPlateHeat.Patch)
                        GetTagTable(Par_Gen.TimeForPlateHeat, res, l, ofs);
                    else if(Patch == Par_Gen.UnloadSpeed.Patch)
                        GetTagTable(Par_Gen.UnloadSpeed, res, l, ofs);

                    else if(Patch == Hmi210_1.Htr1_1.Patch)
                        GetTagTable(Hmi210_1.Htr1_1, res, l, ofs);
                    else if(Patch == Hmi210_1.Htr1_2.Patch)
                        GetTagTable(Hmi210_1.Htr1_2, res, l, ofs);
                    else if(Patch == Hmi210_1.Htr1_3.Patch)
                        GetTagTable(Hmi210_1.Htr1_3, res, l, ofs);
                    else if(Patch == Hmi210_1.Htr1_4.Patch)
                        GetTagTable(Hmi210_1.Htr1_4, res, l, ofs);
                    else if(Patch == Hmi210_1.Htr2_1.Patch)
                        GetTagTable(Hmi210_1.Htr2_1, res, l, ofs);
                    else if(Patch == Hmi210_1.Htr2_2.Patch)
                        GetTagTable(Hmi210_1.Htr2_2, res, l, ofs);
                    else if(Patch == Hmi210_1.Htr2_3.Patch)
                        GetTagTable(Hmi210_1.Htr2_3, res, l, ofs);
                    else if(Patch == Hmi210_1.Htr2_4.Patch)
                        GetTagTable(Hmi210_1.Htr2_4, res, l, ofs);

                    else if(Patch == GenSeqToHmi.CoolTimeAct.Patch)
                        GetTagTable(GenSeqToHmi.CoolTimeAct, res, l, ofs);
                    else if(Patch == GenSeqToHmi.HeatTime_Z1.Patch)
                        GetTagTable(GenSeqToHmi.HeatTime_Z1, res, l, ofs);
                    else if(Patch == GenSeqToHmi.HeatTime_Z2.Patch)
                        GetTagTable(GenSeqToHmi.HeatTime_Z2, res, l, ofs);

                    else if(Patch == GenSeqToHmi.Seq_1_StateNo.Patch)
                        GetTagTable(GenSeqToHmi.Seq_1_StateNo, res, l, ofs);
                    else if(Patch == GenSeqToHmi.Seq_2_StateNo.Patch)
                        GetTagTable(GenSeqToHmi.Seq_2_StateNo, res, l, ofs);
                    else if(Patch == GenSeqToHmi.Seq_3_StateNo.Patch)
                        GetTagTable(GenSeqToHmi.Seq_3_StateNo, res, l, ofs);

                    else if(Patch == GenSeqFromHmi.TempSet1.Patch)
                        GetTagTable(GenSeqFromHmi.TempSet1, res, l, ofs);

                    else if(Patch == AI_Hmi_210.LaminPressBot.Patch)
                        GetTagTable(AI_Hmi_210.LaminPressBot, res, l, ofs);
                    else if(Patch == AI_Hmi_210.LaminPressTop.Patch)
                        GetTagTable(AI_Hmi_210.LaminPressTop, res, l, ofs);
                    else if(Patch == AI_Hmi_210.LAM_TE1.Patch)
                        GetTagTable(AI_Hmi_210.LAM_TE1, res, l, ofs);
                    else if(Patch == AI_Hmi_210.Za_PT3.Patch)
                        GetTagTable(AI_Hmi_210.Za_PT3, res, l, ofs);
                    else if(Patch == AI_Hmi_210.Za_TE3.Patch)
                        GetTagTable(AI_Hmi_210.Za_TE3, res, l, ofs);
                    else if(Patch == AI_Hmi_210.Za_TE4.Patch)
                        GetTagTable(AI_Hmi_210.Za_TE4, res, l, ofs);

                    else if(Patch.find("PLC210 OPC-UA.Application.HMISheetData.Sheet.") != std::string::npos)
                    {
                        GetTagTable(HMISheetData, Patch, res, l, ofs);
                        }
                }
                else if(Patch.find("SPK107 (M01).Application.ForBase_RelFurn_1.") != std::string::npos)
                {
                    GetTagTable(Furn_1, Patch, res, l, ofs);
                }
                else if(Patch.find("SPK107 (M01).Application.ForBase_RelFurn_2.") != std::string::npos)
                {
                    GetTagTable(Furn_2, Patch, res, l, ofs);
                }
                else if(Patch.find("SPK107 (M01)") != std::string::npos)
                {
                    GetTagTable(AllTagPeth, Patch, res, l, ofs);
                }
            }
        }
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQL_LOG, res, comand);
        PQclear(res);

        //for(auto& val : AllTagPeth)
        //{
        //    if(val->Patch.find("1.Data.ProcRun") != std::string::npos)
        //    {
        //        id1ProcRun = val->ID;
        //    }
        //    if(val->Patch.find("1.Data.ProcEnd") != std::string::npos)
        //    {
        //        id1ProcEnd = val->ID;
        //    }
        //    if(val->Patch.find("1.Data.ProcFault") != std::string::npos)
        //    {
        //        id1ProcError = val->ID;
        //    }
        //    if(val->Patch.find("2.Data.ProcRun") != std::string::npos)
        //    {
        //        id2ProcRun = val->ID;
        //    }
        //    if(val->Patch.find("2.Data.ProcEnd") != std::string::npos)
        //    {
        //        id2ProcEnd = val->ID;
        //    }
        //    if(val->Patch.find("2.Data.ProcFault") != std::string::npos)
        //    {
        //        id2ProcError = val->ID;
        //    }
        //}

        //for(auto& val : AllTagKpvl)
        //{
        //    val->UpdateVal();
        //}
        //for(auto& val : AllTagPeth)
        //{
        //    val->UpdateVal();
        //}

        //GetPetch(S107::Furn1::Petch, 1);
        //GetPetch(S107::Furn2::Petch, 2);


        //if(!ofs.bad())
        //    ofs.close();
#pragma endregion

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

        DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, LoginDlg::bagSave);
        if(!conn_spis.connections && !conn_spis.Connection())
            throw std::exception("Error SQL conn_spis connection");

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

