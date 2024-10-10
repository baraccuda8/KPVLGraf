#pragma once

enum class VariantType: uint8_t
{
    NUL = 0,
    BOOLEAN = 1,
    SBYTE = 2,
    BYTE = 3,
    INT16 = 4,
    UINT16 = 5,
    INT32 = 6,
    UINT32 = 7,
    INT64 = 8,
    UINT64 = 9,
    FLOAT = 10,
    DOUBLE = 11,
    STRING = 12,
    DATE_TIME = 13,
    GUId = 14,
    BYTE_STRING = 15,
    XML_ELEMENT = 16,
    NODE_Id = 17,
    EXPANDED_NODE_Id = 18,
    STATUS_CODE = 19,
    QUALIFIED_NAME = 20,
    LOCALIZED_TEXT = 21,
    EXTENSION_OBJECT = 22,
    DATA_VALUE = 23,
    VARIANT = 24,
    DIAGNOSTIC_INFO = 25,
};


class Value{
public:
    boost::any Val;
	int ID = 0;
	bool Arhive = true;
    std::string Patch = "";
	std::string Comment = "";
	float coeff = 1.0;
	float hist = 1.0;
	std::string format = "";
    VariantType Type = VariantType::NUL;
    std::string content = "";

	VariantType GetType() {
        return Type;
	}
    std::string GetString()
    {
        try
        {
                if(format.length())
                {
                    char ss[256] = "";

                    try
                    {
                        if(Type == VariantType::BOOLEAN)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<bool>(Val));
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "BOOLEAN");
                    try
                    {
                        if(Type == VariantType::SBYTE)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<int8_t>(Val) * (int8_t)coeff);
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "SBYTE");
                    try
                    {
                        if(Type == VariantType::BYTE)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<uint8_t>(Val) * (uint8_t)coeff);
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "BYTE");
                    try
                    {
                        //boost::typeindex::type_index t = Val.type();
                        //const char*  n = t.name();
                        if(Type == VariantType::INT16)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<int16_t>(Val) * (int32_t)coeff);
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, Comment);
                    try
                    {
                        if(Type == VariantType::UINT16)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<uint16_t>(Val) * (uint16_t)coeff);
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "UINT16");
                    try
                    {
                        if(Type == VariantType::INT32)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<int32_t>(Val) * (int32_t)coeff);
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "INT32");
                    try
                    {
                        if(Type == VariantType::UINT32)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<uint32_t>(Val) * (uint32_t)coeff);
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "UINT32");
                    try
                    {
                        if(Type == VariantType::INT64)
                        {     
                            sprintf_s(ss, 255, format.c_str(), any_cast<int64_t>(Val) * (int64_t)coeff);
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "INT64");
                    try
                    {
                        if(Type == VariantType::UINT64)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<uint64_t>(Val) * (uint64_t)coeff);
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "UINT64");
                    try
                    {
                        if(Type == VariantType::FLOAT)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<float>(Val) * (float)coeff);
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "FLOAT");
                    try
                    {
                        if(Type == VariantType::DOUBLE)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<double>(Val) * (double)coeff);
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "DOUBLE");
                    try
                    {
                        if(Type == VariantType::STRING)
                        {
                            sprintf_s(ss, 255, format.c_str(), any_cast<std::string>(Val));
                            return ss;
                        }
                    }
                    CATCH(ALL_LOG, "STRING");
                    return ss;
                }
                else
                {
                    try
                    {
                        if(Type == VariantType::STRING)  return content;
                    }
                    CATCH(ALL_LOG, "STRING");
                    try
                    {
                        return content;
                    }CATCH(ALL_LOG, "ANY");
                }
        }
        CATCH(ALL_LOG, "");
        return "";
    }

    Value(){};
    Value(std::string p) : Patch(p) { }

    bool SaveTagTable(std::ofstream& ofs);
    bool GetTagTable(PGresult* res, std::string& patch, int l);



};

//typedef std::deque<Value> VALUE;
//void GetTagTable(PGresult* res, VALUE& All, std::string Patch, int l, std::ofstream& ofs);
//bool GetTagTable(std::string Patch, Value& val, PGresult* res, int l, std::ofstream& ofs)

void SetValue(Value& val);
std::string GetType(VariantType type, std::string value);