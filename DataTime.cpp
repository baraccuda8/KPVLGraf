#include "framework.h"
#include "KPVLGraf.h"
#include "DataTime.h"

time_t DataTimeOfString(std::string str)
{
    try
    {
        std::tm TM;
        TM.tm_year = 0; TM.tm_mon = 0; TM.tm_mday = 0; TM.tm_hour = 0; TM.tm_min = 0; TM.tm_sec = 0;
        std::string::const_iterator start = str.begin();
        std::string::const_iterator end = str.end();
        boost::regex xRegEx(FORMATTIME1);
        boost::match_results<std::string::const_iterator> what;

        if(boost::regex_search(start, end, what, xRegEx, boost::match_default))
        {
            size_t size = what.size();
            if(size > 6)
            {
                if(size > 1)TM.tm_year = Stoi(what[1].str()) - 1900;
                if(size > 2)TM.tm_mon = Stoi(what[2].str()) - 1;
                if(size > 3)TM.tm_mday = Stoi(what[3].str());
                if(size > 4)TM.tm_hour = Stoi(what[4].str());
                if(size > 5)TM.tm_min = Stoi(what[5].str());
                if(size > 6)TM.tm_sec = Stoi(what[6].str());
            }
            return mktime(&TM);
        }
        else
        {
            boost::regex xRegEx(FORMATTIME2);
            boost::match_results<std::string::const_iterator> what;

            if(boost::regex_search(start, end, what, xRegEx, boost::match_default))
            {
                size_t size = what.size();
                if(size > 6)
                {
                    if(size > 1)TM.tm_mday = Stoi(what[1].str());
                    if(size > 2)TM.tm_mon = Stoi(what[2].str()) - 1;
                    if(size > 3)TM.tm_year = Stoi(what[3].str()) - 1900;
                    if(size > 4)TM.tm_hour = Stoi(what[4].str());
                    if(size > 5)TM.tm_min = Stoi(what[5].str());
                    if(size > 6)TM.tm_sec = Stoi(what[6].str());
                }
                return mktime(&TM);
            }
        }

    }
    CATCH(AllLogger, "");

    return 0;
}