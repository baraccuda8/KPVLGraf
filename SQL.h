#pragma once

#include <libpq-fe.h>


#pragma comment(lib, "libpq.lib")

#define SQL_LOG "SQL Log"



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


#define SETUPDATESQL(_l, _c, _s) \
{\
    std::string _comand = _s.str(); \
    if(DEB)LOG_INFO(_l, "{:90}| {}", FUNCTION_LINE_NAME, _comand); \
    PGresult* _res = _c.PGexec(_comand); \
    if(PQresultStatus(_res) == PGRES_FATAL_ERROR)\
        LOG_ERR_SQL(_l, _res, _comand); \
    PQclear(_res);\
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
        std::string ss = PQgetvalue(res, l, i);
        if(!ss.empty())
            return utf8_to_cp1251(ss);
        else return "";
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
