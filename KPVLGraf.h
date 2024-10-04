#pragma once

#include "resource.h"

extern std::string szWindowClass;

#define FUNCTION_LINE_NAME (std::string( __FUNCTION__ ) + std::string (":") + std::to_string(__LINE__))

#define LOG_ERROR(s1, _s2, _s3)DenugInfo(_s1, _s2,  s3);

#define CATCH(_l, _s) \
    catch(std::filesystem::filesystem_error& exc) { DenugInfo(FUNCTION_LINE_NAME, std::string(_s),  exc.what());} \
    catch(std::runtime_error& exc){DenugInfo(FUNCTION_LINE_NAME, std::string(_s), exc.what());} \
    catch(std::exception& exc){DenugInfo(FUNCTION_LINE_NAME, std::string(_s), exc.what());} \
    catch(...){DenugInfo(FUNCTION_LINE_NAME, std::string(_s), "Unknown error");}

inline void DenugInfo(std::string s1, std::string s2, std::string s3)
{
    time_t st = time(0);
    std::tm TM;
    localtime_s(&TM, &st);
    char sFormat[1024];
    sprintf_s(sFormat, 1024, "[%04d-%02d-%02d %02d:%02d:%02d] ", TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);

    std::string file = szWindowClass + ".log";
    std::ofstream F(file.c_str(), std::ios::binary | std::ios::out | std::ios::app);
    if(F.is_open())
    {
        F << sFormat << s1 << " -> " << s2 << " " << s3 << std::endl;
        F.close();
    }
}
