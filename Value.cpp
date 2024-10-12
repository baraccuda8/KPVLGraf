#include "framework.h"
#include "KPVLGraf.h"
#include "SQL.h"
#include "Value.h"



void SetValue(Value& value)
{
    try
    {
             if(value.Type == VariantType::BOOLEAN)   value.Val = (bool)(value.Content == "true");
        else if(value.Type == VariantType::SBYTE)     value.Val = static_cast<int8_t>((std::stoi(value.Content)) / (int8_t)value.Coeff);
        else if(value.Type == VariantType::BYTE)      value.Val = static_cast<uint8_t>((std::stoi(value.Content)) / (uint8_t)value.Coeff);
        else if(value.Type == VariantType::INT16)     value.Val = static_cast<int16_t>((std::stoi(value.Content)) / (int16_t)value.Coeff);
        else if(value.Type == VariantType::UINT16)    value.Val = static_cast<uint16_t>((std::stoi(value.Content)) / (uint16_t)value.Coeff);
        else if(value.Type == VariantType::INT32)     value.Val = static_cast<int32_t>((std::stoi(value.Content)) / (int32_t)value.Coeff);
        else if(value.Type == VariantType::UINT32)    value.Val = static_cast<uint32_t>((std::stol(value.Content)) / (uint32_t)value.Coeff);
        else if(value.Type == VariantType::INT64)     value.Val = static_cast<int64_t>((std::stoll(value.Content)) / (int64_t)value.Coeff);
        else if(value.Type == VariantType::UINT64)    value.Val = static_cast<uint64_t>((std::stoll(value.Content)) / (uint64_t)value.Coeff);
        else if(value.Type == VariantType::FLOAT)     value.Val = static_cast<float>((std::stof(value.Content)) / (float)value.Coeff);
        else if(value.Type == VariantType::DOUBLE)    value.Val = static_cast<double>((std::stof(value.Content)) / (double)value.Coeff);
        else if(value.Type == VariantType::STRING)    value.Val = static_cast<std::string>(cp1251_to_utf8(value.Content));
        value.GetString();
    }CATCH(SQLLogger, "SetValue");
}

std::string GetType(VariantType type)
{
    try
    {
        if(type == VariantType::BOOLEAN)   return "bool";
        else if(type == VariantType::SBYTE)     return "int8_t";
        else if(type == VariantType::BYTE)      return "uint8_t";
        else if(type == VariantType::INT16)     return "int16_t";
        else if(type == VariantType::UINT16)    return "uint16_t";
        else if(type == VariantType::INT32)     return "int32_t";
        else if(type == VariantType::UINT32)    return "uint32_t";
        else if(type == VariantType::INT64)     return "int64_t";
        else if(type == VariantType::UINT64)    return "uint64_t";
        else if(type == VariantType::FLOAT)     return "float";
        else if(type == VariantType::DOUBLE)    return "double";
        else if(type == VariantType::STRING)    return "string";
    }CATCH(SQLLogger, "GetType");
    return "";
}

bool Value::SaveTagTable(std::ofstream& ofs)
{
    try
    {
        if(!ofs.bad())
        {
            ofs << ID << ";";
            ofs << "\"\"\"" << Patch << "\"\"\";";
            ofs << (int)GetType() << ";";
            ofs << Arhive << ";";
            ofs << "\"\"\"" << Comment << "\"\"\";";
            ofs << "(" << ::GetType(Type) << ") " << GetString() << ";";
            ofs << Coeff << ";";
            ofs << Hist << ";";
            ofs << "\"\"\"" << Format << "\"\"\";";
            ofs << std::endl;
        }
        return true;
    }CATCH(SQLLogger, "");
    return false;
}

bool Value::GetTagTable(PGresult* res, int l, std::string& patch)
{
    if(Patch != patch) return false;
    try
    {
        ID = static_cast<int>(Stoi(PGgetvalue(res, l, CollTag::Id).c_str()));
        Arhive = static_cast<bool>(PGgetvalue(res, l, CollTag::Arhive) == "t");
        Comment = static_cast<std::string>(PGgetvalue(res, l, CollTag::Comment));
        Coeff = static_cast<float>(Stof(PGgetvalue(res, l, CollTag::Coeff).c_str()));
        Hist = static_cast<float>(Stod(PGgetvalue(res, l, CollTag::Hist).c_str()));
        Format = static_cast<std::string>(PGgetvalue(res, l, CollTag::Format));

        Type =  static_cast<VariantType>(Stoi(PGgetvalue(res, l, CollTag::Type)));
        Content = static_cast<std::string>(PGgetvalue(res, l, CollTag::Content));
        SetValue(*this);
        AllTag[ID] = this;;
        return true;
    }CATCH(SQLLogger, "GetTagTable");
    return false;
}

bool Value::GetCurentValue(PGresult* res, int l, int id)
{
    if(ID != id) return false;
    try
    {
        Content = static_cast<std::string>(PGgetvalue(res, l, 1));
        SetValue(*this);
    }CATCH(SQLLogger, "GetCurentValue");
    return true;
}



//void GetTagTable(PGresult* res, VALUE& All, std::string Patch, int l, std::ofstream& ofs)
//{
//    try
//    {
//        Value val;
//        val.Patch = Patch;
//        val.ID = atol(PGgetvalue(res, l, CollTag::Id).c_str());
//        val.Arhive = PGgetvalue(res, l, CollTag::Arhive) == "t";
//        val.Comment = PGgetvalue(res, l, CollTag::Comment);
//        val.coeff = static_cast<float>(atof(PGgetvalue(res, l, CollTag::Coeff).c_str()));
//        val.hist = static_cast<float>(atof(PGgetvalue(res, l, CollTag::Hist).c_str()));
//        val.format = PGgetvalue(res, l, CollTag::Format);
//
//        val.Type =  static_cast<VariantType>(Stoi(PGgetvalue(res, l, CollTag::Type)));
//        val.content = PGgetvalue(res, l, CollTag::Content);
//        SetValue(val);
//
//        All.push_back(val);
//
//        //boost::replace_all(value, ".", ",");
//        if(!ofs.bad())
//        {
//            ofs << val.ID << ";";
//            ofs << "\"\"\"" << val.Patch << "\"\"\";";
//            ofs << (int)val.GetType() << ";";
//            ofs << val.Arhive << ";";
//            ofs << "\"\"\"" << val.Comment << "\"\"\";";
//            ofs << GetType(val.Type, val.GetString()) << ";";
//            ofs << val.coeff << ";";
//            ofs << val.hist << ";";
//            ofs << "\"\"\"" << val.format << "\"\"\";";
//            ofs << std::endl;
//        }
//    }CATCH(SQLLogger, "");
//}

//bool GetTagTable(std::string Patch, Value& val, PGresult* res, int l, std::ofstream& ofs)
//{
//    if(val.Patch != Patch) return false;
//    try
//    {
//        val.ID = atol(conn_spis.PGgetvalue(res, l, CollTag::Id).c_str());
//        val.Arhive = conn_spis.PGgetvalue(res, l, CollTag::Arhive) == "t";
//        val.Comment = conn_spis.PGgetvalue(res, l, CollTag::Comment);
//        val.coeff = static_cast<float>(atof(conn_spis.PGgetvalue(res, l, CollTag::Coeff).c_str()));
//        val.hist = static_cast<float>(atof(conn_spis.PGgetvalue(res, l, CollTag::Hist).c_str()));
//        val.format = conn_spis.PGgetvalue(res, l, CollTag::Format);
//
//        val.Type =  static_cast<VariantType>(Stoi(conn_spis.PGgetvalue(res, l, CollTag::Type)));
//        val.content = conn_spis.PGgetvalue(res, l, CollTag::Content);
//        SetValue(val);
//
//        if(!ofs.bad())
//        {
//            ofs << val.ID << ";";
//            ofs << "\"\"\"" << val.Patch << "\"\"\";";
//            ofs << (int)val.GetType() << ";";
//            ofs << val.Arhive << ";";
//            ofs << "\"\"\"" << val.Comment << "\"\"\";";
//            ofs << GetType(val.Type, val.GetString()) << ";";
//            ofs << val.coeff << ";";
//            ofs << val.hist << ";";
//            ofs << "\"\"\"" << val.format << "\"\"\";";
//            ofs << std::endl;
//        }
//        return true;
//    }CATCH(SQLLogger, "");
//    return false;
//}

