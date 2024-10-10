#pragma once

#include "resource.h"

#define MAX_LOADSTRING 100

#define ALL_LOG "All Log"
#define ALL_LOG_REM (std::string(ALL_LOG) + ".log").c_str()

extern HWND GlobalhWnd;
extern HINSTANCE hInstance;

extern std::string szWindowClass;

enum class LOGLEVEL{
    LEVEL_INFO = 0,
    LEVEL_ERROR = 1,
};

#define FUNCTION_LINE_NAME (std::string( __FUNCTION__ ) + std::string (":") + std::to_string(__LINE__))

#define LOG_ERROR(_s1, _s2, _s3)DenugInfo(LOGLEVEL::LEVEL_ERROR, _s1, _s2, _s3);
#define LOG_INFO(_s1, _s2, _s3)DenugInfo(LOGLEVEL::LEVEL_INFO, _s1, _s2, _s3);

#define LOG_ERR_SQL(_l, _r, _c){\
LOG_ERROR(_l, FUNCTION_LINE_NAME, _c);\
LOG_ERROR(_l, FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(_r)));\
}\


#define CATCH(_l, _s) \
    catch(std::filesystem::filesystem_error& exc) { DenugInfo(LOGLEVEL::LEVEL_ERROR, _l, FUNCTION_LINE_NAME, std::string(_s),  exc.what());} \
    catch(std::runtime_error& exc){DenugInfo(LOGLEVEL::LEVEL_ERROR, _l, FUNCTION_LINE_NAME, std::string(_s), exc.what());} \
    catch(std::exception& exc){DenugInfo(LOGLEVEL::LEVEL_ERROR, _l, FUNCTION_LINE_NAME, std::string(_s), exc.what());} \
    catch(...){DenugInfo(LOGLEVEL::LEVEL_ERROR, _l, FUNCTION_LINE_NAME, std::string(_s), "Unknown error");}

LRESULT Quit();

//Вывод строки ошибки выполнения программы
int WinErrorExit(HWND hWnd, const char* lpszFunction);

//Вывод окна в центр родительского окна
BOOL CenterWindow(HWND hwndChild, HWND hwndParent);




//Вывод в файл отладочной ингформации
void DenugInfo(LOGLEVEL l, std::string f, std::string s1, std::string s2, std::string s3 = "");


inline int Stoi(std::string input)
{
    std::optional<int> out = 0;
    try
    {
        return std::stoi(input);
    }
    catch(...) {}
    return 0;
}

inline float Stof(std::string input)
{
    try
    {
        return std::stof(input);
    }
    catch(...) {}
    return 0.0f;
}

inline double Stod(std::string input)
{
    try
    {
        return std::stod(input);
    }
    catch(...) {}
    return 0.0;
}


std::string cp1251_to_utf8(std::string str);
std::string utf8_to_cp1251(std::string str);
