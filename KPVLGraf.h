#pragma once

#include "resource.h"

#define MAX_LOADSTRING 100

#define AllLogger "All Log"
#define AllLogger_Rem (std::string(AllLogger) + ".log").c_str()

extern bool isRun;

extern HWND GlobalhWnd;
extern HINSTANCE hInstance;

extern std::string szWindowClass;

enum class LOGLEVEL{
    LEVEL_INFO = 0,
    LEVEL_WARN = 1,
    LEVEL_ERROR = 2,
};

#define FUNCTION_LINE_NAME (std::string( __FUNCTION__ ) + std::string (":") + std::to_string(__LINE__))

#define LOG_INFO(_l, _s2, _c)DenugInfo(LOGLEVEL::LEVEL_INFO, _l, _s2, _c)
#define LOG_WARN(_l, _s2, _c)DenugInfo(LOGLEVEL::LEVEL_WARN, _l, _s2, _c)
#define LOG_ERROR(_l, _s2, _c)DenugInfo(LOGLEVEL::LEVEL_ERROR, _l, _s2, _c)

#define LOG_ERR_SQL(_l, _r, _c){\
LOG_ERROR(_l, FUNCTION_LINE_NAME, _c);\
LOG_ERROR(_l, FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(_r)));\
}\



#define CATCH(_l, _s) \
    catch(std::filesystem::filesystem_error& exc) { DenugInfo(LOGLEVEL::LEVEL_ERROR, _l, FUNCTION_LINE_NAME, std::string(_s) + ": " + exc.what());} \
    catch(std::runtime_error& exc){DenugInfo(LOGLEVEL::LEVEL_ERROR, _l, FUNCTION_LINE_NAME, std::string(_s) + ": " + exc.what());} \
    catch(std::exception& exc){DenugInfo(LOGLEVEL::LEVEL_ERROR, _l, FUNCTION_LINE_NAME, std::string(_s) + ": " + exc.what());} \
    catch(...){DenugInfo(LOGLEVEL::LEVEL_ERROR, _l, FUNCTION_LINE_NAME, std::string(_s) + ": " + "Unknown error");}

#define WaitCloseTheread(_t) if(_t){\
    DWORD dwEvent = WaitForSingleObject(_t, 2000);\
    if(dwEvent == WAIT_OBJECT_0)        LOG_INFO(AllLogger, FUNCTION_LINE_NAME, std::string(#_t) + " = WAIT_OBJECT_0");\
    else if(dwEvent == WAIT_ABANDONED)  LOG_WARN(AllLogger, FUNCTION_LINE_NAME, std::string(#_t) + " = WAIT_ABANDONED");\
    else if(dwEvent == WAIT_TIMEOUT)    LOG_WARN(AllLogger, FUNCTION_LINE_NAME, std::string(#_t) + " = WAIT_TIMEOUT");\
    else if(dwEvent == WAIT_FAILED)     LOG_WARN(AllLogger, FUNCTION_LINE_NAME, std::string(#_t) + " = WAIT_FAILED");\
    else                                LOG_WARN(AllLogger, FUNCTION_LINE_NAME, std::string(#_t) + " = " + std::to_string(dwEvent));\
}



LRESULT Quit();

//Вывод строки ошибки выполнения программы
int WinErrorExit(HWND hWnd, const char* lpszFunction);

//Вывод окна в центр родительского окна
BOOL CenterWindow(HWND hwndChild, HWND hwndParent);




//Вывод в файл отладочной ингформации
void DenugInfo(LOGLEVEL l, std::string f, std::string s1, std::string s2);
void DenugInfo(LOGLEVEL l, std::string f, std::string s1, std::stringstream& s2);


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
