#include "framework.h"
#include "KPVLGraf.h"
#include "DataTime.h"

time_t DataTimeOfString(std::string str)
{
    try
    {
        std::tm TM ={0,0,0,0,0,0,0,0,0};
        std::string::const_iterator start = str.begin();
        std::string::const_iterator end = str.end();
        boost::match_results<std::string::const_iterator> what;

        boost::regex xRegEx(FORMATTIME1);
        if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 6)
        {
            TM.tm_year = Stoi(what[1]) - 1900;
            TM.tm_mon = Stoi(what[2]) - 1;
            TM.tm_mday = Stoi(what[3]);
            TM.tm_hour = Stoi(what[4]);
            TM.tm_min = Stoi(what[5]);
            TM.tm_sec = Stoi(what[6]);
            return mktime(&TM);
        }
        else
        {
            xRegEx = FORMATTIME2;
            boost::match_results<std::string::const_iterator> what;

            if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 6)
            {
                TM.tm_mday = Stoi(what[1]);
                TM.tm_mon = Stoi(what[2]) - 1;
                TM.tm_year = Stoi(what[3]) - 1900;
                TM.tm_hour = Stoi(what[4]);
                TM.tm_min = Stoi(what[5]);
                TM.tm_sec = Stoi(what[6]);
                return mktime(&TM);
            }
        }

    }
    CATCH(AllLogger, "");

    return 0;
}