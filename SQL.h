#pragma once

#include <libpq-fe.h>
#include "Value.h"

#pragma comment(lib, "libpq.lib")


#pragma region define

#define    SQL_LOG "SQL Log"
#define    SheetPos0 0
#define    SheetPos1 1
#define    SheetPos2 2
#define    SheetPos3 3
#define    SheetPos4 4
#define    SheetPos5 5
#define    SheetPos6 6
#define    SheetMaxPos 7


#define    Cassette1 0
#define    Cassette2 1
#define    Cassette3 2
#define    Cassette4 3
#define    Cassette5 4
#define    Cassette6 5
#define    Cassette7 6
#define    MaxCassette 7


#define SETUPDATESQL(_l, _c, _s) \
{\
    std::string _comand = _s.str(); \
    if(DEB)LOG_INFO(_l, "{:90}| {}", FUNCTION_LINE_NAME, _comand); \
    PGresult* _res = _c.PGexec(_comand); \
    if(PQresultStatus(_res) == PGRES_FATAL_ERROR)\
        LOG_ERR_SQL(_l, _res, _comand); \
    PQclear(_res);\
}
#pragma endregion


extern std::string m_dbhost;
extern std::string m_dbport;
extern std::string m_dbname;
extern std::string m_dbuser;
extern std::string m_dbpass;

std::string cp1251_to_utf8(std::string str);
std::string utf8_to_cp1251(std::string str);


namespace CollTag{
    extern int Id;
    extern int Name;
    extern int Type;
    extern int Arhive;
    extern int Comment;
    extern int Content;
    extern int Coeff;
    extern int Hist;
    extern int Format;
    extern int Idsec;
}



inline std::string PGgetvalue(PGresult* res, int l, int i)
{
    std::string ss = PQgetvalue(res, l, i);
    if(!ss.empty())
        return utf8_to_cp1251(ss);
    else return "";
}

class PGConnection
{
public:
    PGconn* m_connection;
    bool connections = false;
    std::string Name = "";
    PGConnection() { };

    ~PGConnection()
    { 
        if(connections)PQfinish(m_connection); 
        connections = false; 
    };

    bool Connection(){
        try
        {
            if(connections)return connections;

            m_connection = PQsetdbLogin(
                m_dbhost.c_str(),
                m_dbport.c_str(),
                nullptr, nullptr,
                m_dbname.c_str(),
                m_dbuser.c_str(),
                m_dbpass.c_str());

            ConnStatusType st = PQstatus(m_connection);
            if(st != CONNECTION_OK && PQsetnonblocking(m_connection, 1) != 0)
            {
                connections = false;
                throw std::runtime_error(PQerrorMessage(m_connection));
            }

            PGresult* res = PQexec(m_connection, "set time zone 'Asia/Yekaterinburg'");
            ExecStatusType sd = PQresultStatus(res);
            if(sd != PGRES_COMMAND_OK)
            {
                std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                throw std::runtime_error(errc);
            }
            PQclear(res);

            connections = true;
        }CATCH(SQL_LOG, "")

        return connections;
    }
    
    int PQntuples(PGresult* res)
    {
        if(!connections) return 0;
        return ::PQntuples(res);
    }

    PGresult* PGexec(std::string std){ 
        if(!connections) return 0;
        return 
            ::PQexec(m_connection, cp1251_to_utf8(std).c_str());
    };

    PGresult* MyPQexec(std::string std){
        if(!connections) return 0;
        return
            ::PQexec(m_connection, std.c_str());
    };

    std::string PGgetvalue(PGresult* res, int l, int i)
    {
        if(!connections) return "";
        return ::PGgetvalue(res, l, i);

        //std::string ss = PQgetvalue(res, l, i);
        //if(!ss.empty())
            //return utf8_to_cp1251(ss);
        //else return "";
    }

};


//extern std::string PLC210APPL;
class T_Tag{
public:
    virtual bool GetTagTable(PGresult* res, std::string& patch, int l) = 0;
};


class T_AI_Hmi_210 : public T_Tag{
    std::string pt = "";
public:
    T_AI_Hmi_210(std::string p): pt(p){ }

    //REAL Давление воды закалка коллектор низ
    Value LaminPressBot = Value(pt + "LaminPressBot.AI_eu");

    //REAL Давление воды закалка коллектор верх
    Value LaminPressTop = Value(pt + "LaminPressTop.AI_eu");

    //REAL Температура воды в поддоне
    Value LAM_TE1 = Value(pt + "LAM_TE1.AI_eu");

    //REAL Температура воды в баке
    Value Za_PT3 = Value(pt + "Za_PT3.AI_eu");

    //REAL Температура воды в баке
    Value Za_TE3 = Value(pt + "Za_TE3.AI_eu");

    //REAL Пирометр
    Value Za_TE4 = Value(pt + "Za_TE4.AI_eu");

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;
        if(LaminPressBot.GetTagTable(res, patch, l))return true;
        if(LaminPressTop.GetTagTable(res, patch, l))return true;
        if(LAM_TE1.GetTagTable(res, patch, l))return true;
        if(Za_PT3.GetTagTable(res, patch, l))return true;
        if(Za_TE3.GetTagTable(res, patch, l))return true;
        if(Za_TE4.GetTagTable(res, patch, l))return true;
        return false;
    }
};

class T_GenSeqFromHmi: public T_Tag{
    std::string pt = "";
public:
    T_GenSeqFromHmi(std::string p): pt(p) { }

    //REAL Уставки температуры в печи
    Value TempSet1 = Value(pt + "TempSet1");

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;
        if(TempSet1.GetTagTable(res, patch, l))return true;
        return false;
    }
};

class T_GenSeqToHmi: public T_Tag{
    std::string pt = "";
public:
    T_GenSeqToHmi(std::string p): pt(p) { }

    //REAL Сколько прошло времени охлаждения
    Value CoolTimeAct = Value(pt + "CoolTimeAct");

    //REAL время нагрева в зоне 1
    Value HeatTime_Z1 = Value(pt + "HeatTime_Z1");

    //REAL время нагрева в зоне 2
    Value HeatTime_Z2 = Value(pt + "HeatTime_Z2");

    //INT Номер шага последовательности загрузки в печь
    Value Seq_1_StateNo = Value(pt + "Seq_1_StateNo");

    //INT Номер шага последовательности выгрузки в печи
    Value Seq_2_StateNo = Value(pt + "Seq_2_StateNo");

    //INT Номер шага последовательности обработки в ламинарке
    Value Seq_3_StateNo = Value(pt + "Seq_3_StateNo");

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;
        if(CoolTimeAct.GetTagTable(res, patch, l))return true;
        if(HeatTime_Z1.GetTagTable(res, patch, l))return true;
        if(HeatTime_Z2.GetTagTable(res, patch, l))return true;
        if(Seq_1_StateNo.GetTagTable(res, patch, l))return true;
        if(Seq_2_StateNo.GetTagTable(res, patch, l))return true;
        if(Seq_3_StateNo.GetTagTable(res, patch, l))return true;
        return false;
    }
};

class T_Par_Gen: public T_Tag{
    std::string pt = "";
public:
    T_Par_Gen(std::string p): pt(p) { }

    //REAL Уставка давления для запуска комперссора
    Value PresToStartComp = Value(pt + "PresToStartComp");

    //REAL Время сигнализации окончания нагрева, мин
    Value TimeForPlateHeat = Value(pt + "TimeForPlateHeat");

    //REAL Скорость выгрузки
    Value UnloadSpeed = Value(pt + "UnloadSpeed");

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;
        if(PresToStartComp.GetTagTable(res, patch, l))return true;
        if(TimeForPlateHeat.GetTagTable(res, patch, l))return true;
        if(UnloadSpeed.GetTagTable(res, patch, l))return true;
        return false;
    }
};

class T_Hmi210_1: public T_Tag{
    std::string pt = "";
public:
    T_Hmi210_1(std::string p): pt(p) { }

    //REAL Температура в зоне 1.1
    Value Htr1_1 = Value (pt + "Htr_1.ToHmi.TAct");

    //REAL Температура в зоне 1.2
    Value Htr1_2 = Value (pt + "Htr_2.ToHmi.TAct");

    //REAL Температура в зоне 1.3
    Value Htr1_3 = Value (pt + "Htr_3.ToHmi.TAct");

    //REAL Температура в зоне 1.4
    Value Htr1_4 = Value (pt + "Htr_4.ToHmi.TAct");

    //REAL Температура в зоне 2.1
    Value Htr2_1 = Value (pt + "Htr2_1.ToHmi.TAct");

    //REAL Температура в зоне 2.2
    Value Htr2_2 = Value (pt + "Htr2_2.ToHmi.TAct");

    //REAL Температура в зоне 2.3
    Value Htr2_3 = Value (pt + "Htr2_3.ToHmi.TAct");

    //REAL Температура в зоне 2.4
    Value Htr2_4 = Value (pt + "Htr2_4.ToHmi.TAct");

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;
        if(Htr1_1.GetTagTable(res, patch, l))return true;
        if(Htr1_2.GetTagTable(res, patch, l))return true;
        if(Htr1_3.GetTagTable(res, patch, l))return true;
        if(Htr1_4.GetTagTable(res, patch, l))return true;
        if(Htr2_1.GetTagTable(res, patch, l))return true;
        if(Htr2_2.GetTagTable(res, patch, l))return true;
        if(Htr2_3.GetTagTable(res, patch, l))return true;
        if(Htr2_4.GetTagTable(res, patch, l))return true;
        return false;
    }
};

class T_PlateData: public T_Tag{
    std::string pt = "";
public:
    T_PlateData(std::string p): pt(p) { }

    //Char Код марки
    Value AlloyCodeText = Value(pt + "AlloyCodeText.sText");

    //DINT номер плавки
    Value Melt = Value(pt + "Melt");

    //DINT Пачка
    Value Pack = Value(pt + "Pack");

    //DINT номер партии
    Value PartNo = Value(pt + "PartNo");

    //DINT номер листа
    Value Sheet = Value(pt + "Sheet");

    //DINT Номер сляба
    Value Slab = Value(pt + "Slab");

    //DINT подномер листа
    Value SubSheet = Value(pt + "SubSheet");

    //Char код толщины. Зависит от марки стали.
    Value ThiknessText = Value(pt + "ThiknessText.sText");

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;

        if(AlloyCodeText.GetTagTable(res, patch, l))return true;
        if(Melt.GetTagTable(res, patch, l))return true;
        if(Pack.GetTagTable(res, patch, l))return true;
        if(PartNo.GetTagTable(res, patch, l))return true;
        if(Sheet.GetTagTable(res, patch, l))return true;
        if(Slab.GetTagTable(res, patch, l))return true;
        if(SubSheet.GetTagTable(res, patch, l))return true;
        if(ThiknessText.GetTagTable(res, patch, l))return true;
        return false;
    }
};

class T_CassetteData: public T_Tag{
    std::string pt = "";
public:
    T_CassetteData(std::string p): pt(p) { }

    //DINT Номер кассеты за день
    Value CassetteNo = Value(pt + "CassetteNo");

    //DINT День ID листа
    Value Day = Value(pt + "Day");

    //DINT Месяц ID листа
    Value Month = Value(pt + "Month");

    //INT Номер листа в касете
    Value SheetInCassette = Value(pt + "SheetInCassette");

    //DINT Год ID листа
    Value Year = Value(pt + "Year");

    //UINT Час ID листа
    Value Hour = Value(pt + "Hour");

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;

        if(Year.GetTagTable(res, patch, l))return true;
        if(Month.GetTagTable(res, patch, l))return true;
        if(Day.GetTagTable(res, patch, l))return true;
        if(Hour.GetTagTable(res, patch, l))return true;

        if(CassetteNo.GetTagTable(res, patch, l))return true;
        if(SheetInCassette.GetTagTable(res, patch, l))return true;

        return false;
    }
};

class  T_Side: public T_Tag{
    std::string pt = "";
public:
    T_Side(std::string p): pt(p) { }

    //REAL высота листа в точке1
    Value h1 = Value (pt + "h1");

    //REAL высота листа в точке2
    Value h2 = Value (pt + "h2");

    //REAL высота листа в точке3
    Value h3 = Value (pt + "h3");

    //REAL высота листа в точке4
    Value h4 = Value (pt + "h4");

    //REAL высота листа в точке5
    Value h5 = Value (pt + "h5");

    //REAL высота листа в точке6
    Value h6 = Value (pt + "h6");

    //REAL высота листа в точке7
    Value h7 = Value (pt + "h7");

    //REAL высота листа в точке8
    Value h8 = Value (pt + "h8");
    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;

        if(h1.GetTagTable(res, patch, l))return true;
        if(h2.GetTagTable(res, patch, l))return true;
        if(h3.GetTagTable(res, patch, l))return true;
        if(h4.GetTagTable(res, patch, l))return true;
        if(h5.GetTagTable(res, patch, l))return true;
        if(h6.GetTagTable(res, patch, l))return true;
        if(h7.GetTagTable(res, patch, l))return true;
        if(h8.GetTagTable(res, patch, l))return true;
        return false;
    }
};

class T_Section: public T_Tag{
    std::string pt = "";
public:
    T_Section(std::string p): pt(p) { }

    //Низ
    Value Bottom = Value(pt + "Bottom");

    //Верх
    Value Top = Value(pt + "Top");

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;
        if(Bottom.GetTagTable(res, patch, l))return true;
        if(Top.GetTagTable(res, patch, l))return true;
        return false;
    }
};

class T_HMISheetData: public T_Tag{
    std::string pt = "";
public:
    T_HMISheetData(std::string p): pt(p) { }

    //DINT Счетчик циклов контроллера
    //Value WDG = Value(pt + "");

    //BOOL Бит жизни для базы
    //Value WDG_toBase = Value(pt + "");

    //BOOL Обратный бит жизни для контроллера
    //Value WDG_fromBase = Value(pt + "");

    //BOOL Новые лист в касету
    Value NewData = Value(pt + "NewData");

    //BOOL Ответ Новые лист в касету
    //Value SaveDone = Value(pt + "SaveDone");

    //BOOL кассета наполяентся
    Value CasseteIsFill = Value(pt + "CasseteIsFill");

    //BOOL Кассета готова
    Value CassetteIsComplete = Value(pt + "CassetteIsComplete");

    //BOOL Начать новую кассету
    Value StartNewCassette = Value(pt + "StartNewCassette");

    //WORD Кларана верх
    Value Valve_1x = Value(pt + "Valve_1x");

    //WORD Клапна низ
    Value Valve_2x = Value(pt + "Valve_2x");;

    //Клапан. Скоростная секция
    T_Section SpeedSection = T_Section (pt + "SpeedSection.");

    //Клапан. Ламинарная секция 1
    T_Section LaminarSection1 = T_Section(pt + "LaminarSection1.");

    //Клапан. Ламинарная секция 2
    T_Section LaminarSection2 = T_Section(pt + "LaminarSection2.");

    //Откланение до кантовки
    T_Side Top_Side = T_Side(pt + "Top_Side.");

    //Откланение после кантовки
    T_Side Bot_Side = T_Side(pt + "Bottom_Side.");

    //Наполняемая кассета на кантовке
    T_CassetteData Cassette = T_CassetteData(pt + "Cassette.");

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;

        if(NewData.GetTagTable(res, patch, l))return true;
        if(CasseteIsFill.GetTagTable(res, patch, l))return true;
        if(CassetteIsComplete.GetTagTable(res, patch, l))return true;
        if(StartNewCassette.GetTagTable(res, patch, l))return true;
        if(Valve_1x.GetTagTable(res, patch, l))return true;
        if(Valve_2x.GetTagTable(res, patch, l))return true;

        if(SpeedSection.GetTagTable(res, patch, l))return true;
        if(LaminarSection1.GetTagTable(res, patch, l))return true;
        if(LaminarSection2.GetTagTable(res, patch, l))return true;

        if(Top_Side.GetTagTable(res, patch, l))return true;
        if(Bot_Side.GetTagTable(res, patch, l))return true;

        if(Cassette.GetTagTable(res, patch, l))return true;

        return false;
    }
};

class T_FCassette: public T_Tag{
    std::string pt = "";
public:
    T_FCassette(std::string p): pt(p) { }

    //DINT Год ID листа
    Value Year = Value(pt + "Year");

    //DINT Месяц ID листа
    Value Month = Value(pt + "Month");

    //DINT День ID листа
    Value Day = Value(pt + "Day");

    //DINT Номер кассеты за день
    Value CassetteNo = Value(pt + "CaasetteNo");

    //UINT Час ID листа
    Value Hour = Value(pt + "Hour");
    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;

        if(Year.GetTagTable(res, patch, l))return true;
        if(Month.GetTagTable(res, patch, l))return true;
        if(Day.GetTagTable(res, patch, l))return true;
        if(Hour.GetTagTable(res, patch, l))return true;

        if(CassetteNo.GetTagTable(res, patch, l))return true;

        return false;
    }
};

class T_Furn: public T_Tag{
    std::string pt = "";
public:
    T_Furn(std::string p): pt(p) { }
    
    //REAL Время до окончания процесса, мин
    Value TimeToProcEnd = Value(pt + "TimeToProcEnd");

    //REAL Полное время процесса (уставка), мин
    Value TimeProcSet = Value(pt + "TimeProcSet");

    //REAL Заданное значение температуры
    Value TempRef = Value(pt + "TempRef");

    //REAL Время процесса, час (0..XX)
    Value ProcTimeMin = Value(pt + "ProcTimeMin");

    //BOOL Работа
    Value ProcRun = Value(pt + "ProcRun");

    //BOOL Авария процесса
    Value ProcFault = Value(pt + "ProcFault");

    //BOOL Окончание процесса
    Value ProcEnd = Value(pt + "ProcEnd");

    //REAL Задание Время разгона
    Value PointTime_1 = Value(pt + "PointTime_1");

    //REAL Уставка температуры
    Value PointRef_1 = Value(pt + "PointRef_1");

    //REAL Задание Время выдержки
    Value PointDTime_2 = Value(pt + "PointDTime_2");

    //REAL Фактическое значение температуры
    Value TempAct = Value(pt + "TempAct");

    //REAL Термопара 2
    Value T2 = Value(pt + "T2");

    //REAL Термопара 1
    Value T1 = Value(pt + "T1");

    //REAL Факт время нагрева
    Value ActTimeHeatAcc = Value(pt + "ActTimeHeatAcc");

    //REAL Факт время выдержки
    Value ActTimeHeatWait = Value(pt + "ActTimeHeatWait");

    //REAL Факт общее время 
    Value ActTimeTotal = Value(pt + "ActTimeTotal");

    //Возврат касеты в список
    Value ReturnCassetteCmd = Value(pt + "ReturnCassetteCmd");

    //Кассета на отпуске
    T_FCassette Cassette = T_FCassette(pt + "Cassette.");;

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;

        if(PointTime_1.GetTagTable(res, patch, l))return true;
        if(PointRef_1.GetTagTable(res, patch, l))return true;
        if(PointDTime_2.GetTagTable(res, patch, l))return true;
        if(ProcRun.GetTagTable(res, patch, l))return true;
        if(ProcEnd.GetTagTable(res, patch, l))return true;
        if(ProcFault.GetTagTable(res, patch, l))return true;

        if(TimeProcSet.GetTagTable(res, patch, l))return true;
        if(ProcTimeMin.GetTagTable(res, patch, l))return true;
        if(TimeToProcEnd.GetTagTable(res, patch, l))return true;

        if(TempRef.GetTagTable(res, patch, l))return true;
        if(TempAct.GetTagTable(res, patch, l))return true;

        if(T1.GetTagTable(res, patch, l))return true;
        if(T2.GetTagTable(res, patch, l))return true;

        if(ActTimeHeatAcc.GetTagTable(res, patch, l))return true;
        if(ActTimeHeatWait.GetTagTable(res, patch, l))return true;
        if(ActTimeTotal.GetTagTable(res, patch, l))return true;

        if(ReturnCassetteCmd.GetTagTable(res, patch, l))return true;

        if(Cassette.GetTagTable(res, patch, l))return true;

        return false;
    }
};


class T_CassetteArray: public T_Tag{
    std::string pt = "";
public:
    T_CassetteArray(std::string p): pt(p) { }

    //Выбор кассеты в 1-й печи
    Value selected_cassetFurn1 = Value(pt + "selected_cassetFurn1");

    //Выбор кассеты в 2-й печи
    Value selected_cassetFurn2 = Value(pt + "selected_casset");

    T_FCassette Cassette[MaxCassette] ={
        T_FCassette(pt + "cassette[1]."),
        T_FCassette(pt + "cassette[2]."),
        T_FCassette(pt + "cassette[3]."),
        T_FCassette(pt + "cassette[4]."),
        T_FCassette(pt + "cassette[5]."),
        T_FCassette(pt + "cassette[6]."),
        T_FCassette(pt + "cassette[7]."),
    };

    bool GetTagTable(PGresult* res, std::string& patch, int l)
    {
        if(patch.find(pt) == std::string::npos)return false;

        if(selected_cassetFurn1.GetTagTable(res, patch, l))return true;
        if(selected_cassetFurn2.GetTagTable(res, patch, l))return true;

        if(Cassette[Cassette1].GetTagTable(res, patch, l))return true;
        if(Cassette[Cassette2].GetTagTable(res, patch, l))return true;
        if(Cassette[Cassette3].GetTagTable(res, patch, l))return true;
        if(Cassette[Cassette4].GetTagTable(res, patch, l))return true;
        if(Cassette[Cassette5].GetTagTable(res, patch, l))return true;
        if(Cassette[Cassette6].GetTagTable(res, patch, l))return true;
        if(Cassette[Cassette7].GetTagTable(res, patch, l))return true;

        return false;
    }
};

void InitCurentTag();
bool InitSQL();

//extern PGConnection conn_kpvl;
//extern PGConnection conn_kpvl2;
//
//extern PGConnection conn_dops;
//extern PGConnection conn_temp;
//extern PGConnection conn_spis;
//extern PGConnection conn_spic;
//
//extern PGConnection conn_grqff1;
//extern PGConnection conn_grqff2;
//extern PGConnection conn_grqff3;
//
