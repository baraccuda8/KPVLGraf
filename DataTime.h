#pragma once
const std::string FORMATTIME1 = "^(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}):(\\d{2}):(\\d{2})*";
const std::string FORMATTIME2 = "^(\\d{2})-(\\d{2})-(\\d{4}) (\\d{2}):(\\d{2}):(\\d{2})*";

time_t DataTimeOfString(std::string str);
