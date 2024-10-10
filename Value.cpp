#include "framework.h"
#include "KPVLGraf.h"
#include "SQL.h"
#include "Value.h"



void SetValue(Value& val)
{
    try
    {
        if(val.Type == VariantType::BOOLEAN)   val.Val = (bool)(val.content == "true");      // atoi_t(bool, atoi, value);
        else if(val.Type == VariantType::SBYTE)     val.Val = static_cast<int8_t>((std::stoi(val.content)) / (int8_t)val.coeff);
        else if(val.Type == VariantType::BYTE)      val.Val = static_cast<uint8_t>((std::stoi(val.content)) / (uint8_t)val.coeff);
        else if(val.Type == VariantType::INT16)     val.Val = static_cast<int16_t>((std::stoi(val.content)) / (int16_t)val.coeff);
        else if(val.Type == VariantType::UINT16)    val.Val = static_cast<uint16_t>((std::stoi(val.content)) / (uint16_t)val.coeff);
        else if(val.Type == VariantType::INT32)     val.Val = static_cast<int32_t>((std::stoi(val.content)) / (int32_t)val.coeff);
        else if(val.Type == VariantType::UINT32)    val.Val = static_cast<uint32_t>((std::stol(val.content)) / (uint32_t)val.coeff);
        else if(val.Type == VariantType::INT64)     val.Val = static_cast<int64_t>((std::stoll(val.content)) / (int64_t)val.coeff);
        else if(val.Type == VariantType::UINT64)    val.Val = static_cast<uint64_t>((std::stoll(val.content)) / (uint64_t)val.coeff);
        else if(val.Type == VariantType::FLOAT)     val.Val = static_cast<float>((std::stof(val.content)) / (float)val.coeff);
        else if(val.Type == VariantType::DOUBLE)    val.Val = static_cast<double>((std::stof(val.content)) / (double)val.coeff);
        else if(val.Type == VariantType::STRING)    val.Val = static_cast<std::string>(cp1251_to_utf8(val.content)); // utf8_to_cp1251(value); //cp1251_to_utf8
        val.GetString();
    }CATCH(SQL_LOG, "");
}

std::string GetType(VariantType type, std::string value)
{
    try
    {
        if(type == VariantType::BOOLEAN)   return "(bool)" + value;
        else if(type == VariantType::SBYTE)     return "(int8_t)" + value;
        else if(type == VariantType::BYTE)      return "(uint8_t)" + value;
        else if(type == VariantType::INT16)     return "(int16_t)" + value;
        else if(type == VariantType::UINT16)    return "(uint16_t)" + value;
        else if(type == VariantType::INT32)     return "(int32_t)" + value;
        else if(type == VariantType::UINT32)    return "(uint32_t)" + value;
        else if(type == VariantType::INT64)     return "(int64_t)" + value;
        else if(type == VariantType::UINT64)    return "(uint64_t)" + value;
        else if(type == VariantType::FLOAT)     return "(float)" + value;
        else if(type == VariantType::DOUBLE)    return "(double)" + value;
        else if(type == VariantType::STRING)    return "std::string(\"" + value + "\")";
    }CATCH(SQL_LOG, "");
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
            ofs << ::GetType(Type, GetString()) << ";";
            ofs << coeff << ";";
            ofs << hist << ";";
            ofs << "\"\"\"" << format << "\"\"\";";
            ofs << std::endl;
        }
        return true;
    }CATCH(SQL_LOG, "");
    return false;
}

bool Value::GetTagTable(PGresult* res, std::string& patch, int l)
{
    if(Patch != patch) return false;
    try
    {
        ID = static_cast<int>(Stoi(PGgetvalue(res, l, CollTag::Id).c_str()));
        Arhive = static_cast<bool>(PGgetvalue(res, l, CollTag::Arhive) == "t");
        Comment = static_cast<std::string>(PGgetvalue(res, l, CollTag::Comment));
        coeff = static_cast<float>(Stof(PGgetvalue(res, l, CollTag::Coeff).c_str()));
        hist = static_cast<float>(Stod(PGgetvalue(res, l, CollTag::Hist).c_str()));
        format = static_cast<std::string>(PGgetvalue(res, l, CollTag::Format));

        Type =  static_cast<VariantType>(Stoi(PGgetvalue(res, l, CollTag::Type)));
        content = static_cast<std::string>(PGgetvalue(res, l, CollTag::Content));
        SetValue(*this);
        return true;
    }CATCH(SQL_LOG, "");
    return false;
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
//    }CATCH(SQL_LOG, "");
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
//    }CATCH(SQL_LOG, "");
//    return false;
//}

