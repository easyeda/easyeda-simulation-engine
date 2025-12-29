/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include"strBaseProp.h"
static void replaceAll(std::string& str, const std::string& from, const std::string& to);

std::string strBaseProp::getStr(std::string str)
{
    replaceAll(str, "\n", "&#xa;");
    replaceAll(str, "\"", "&#x22;");
    replaceAll(str, "<", "&#x3C;");
    replaceAll(str, "=", "&#x3D;");
    replaceAll(str, ">", "&#x3E;");
    return str;
}

std::string strBaseProp::setStr(std::string str)
{
    replaceAll(str, "&#xa;", "\n");
    replaceAll(str, "&#x22;", "\"");
    replaceAll(str, "&#x3C;", "<");
    replaceAll(str, "&#x3D;", "=");
    replaceAll(str, "&#x3E;", ">");
    return str;
}

double strBaseProp::getValue()
{
    std::string str = getValStr(); // Assuming getValStr() returns a std::string
    return std::stod(str); // Convert string to double
}

//在字符串中查找和替换所有出现的子字符串。
static void replaceAll(std::string& str, const std::string& from, const std::string& to) 
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) 
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
