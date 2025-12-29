/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

//#include "mainwindow.h"
#include "utils.h"
#include "pin.h"


const std::unordered_map<std::string, double> multipliers = {
    // 基础前缀单位
    {"p", 1e-12}, {"n", 1e-9}, {"u", 1e-6}, {"m", 1e-3},
    {"k", 1e3}, {"M", 1e6}, {"G", 1e9}, {"T", 1e12},
    
    // SPICE分析专用单位
    {"s", 1.0}, {"ms", 1e-3}, {"us", 1e-6}, {"ns", 1e-9}, {"ps", 1e-12},
    {"Hz", 1.0}, {"kHz", 1e3}, {"MHz", 1e6}, {"GHz", 1e9}
};

double getMultiplier(std::string& unit) {
    if (unit.empty()) return 1.0;
    
    std::string lowerUnit = unit;
    std::transform(lowerUnit.begin(), lowerUnit.end(), lowerUnit.begin(), ::tolower);
    
    for (const auto& pair : multipliers) {
        std::string unitKey = pair.first;
        std::transform(unitKey.begin(), unitKey.end(), unitKey.begin(), ::tolower);
        
        if (lowerUnit.find(unitKey) != std::string::npos) {
            if (unitKey.find("s") != std::string::npos || 
                unitKey.find("hz") != std::string::npos) {
                return pair.second;
            }
        }
    }
    
    char firstChar = unit[0];
    auto it = multipliers.find(std::string(1, firstChar));
    return (it != multipliers.end()) ? it->second : 1.0;
}
double getValueOfNum(std::string& val) {
    if (val.empty()) return 0.0;
    
    // 提取数值部分
    size_t pos = 0;
    double number = 0.0;
    
    try {
        number = std::stod(val, &pos);
    } catch (const std::exception& e) {
        if (val.find("AC") != std::string::npos || 
            val.find("DC") != std::string::npos) {
            return 0.0;
        }
        std::cerr << "Numerical conversion error: " << val << " - " << e.what() << "\n";
        return 0.0;
    }
    
    // 提取单位部分 (跳过分隔空格)
    while (pos < val.size() && std::isspace(val[pos])) pos++;
    std::string unit = (pos < val.size()) ? val.substr(pos) : "";
    
    return number * getMultiplier(unit);
}

std::string multToValStr(double value, std::string mult)
{
    std::ostringstream oss;
    oss << value * getMultiplier(mult);
    return oss.str();
}

std::string val2hex(int d)
{
    std::stringstream ss;
    ss << std::uppercase << std::hex << d;
    std::string h = ss.str();
    if (h.size() % 2) h = "0" + h;
    return h;
}

std::string toDigit(int d)
{
    const std::string Hex = "0123456789ABCDEF";
    std::string h;
    do {
        h = Hex[d & 0x0F] + h;
        d >>= 4;
    } while (d != 0);
    return h;
}

std::string decToBase(int value, int base, int digits)
{
    std::string converted = "";
    for (int i = 0; i < digits; i++)
    {
        if (value >= base) converted = toDigit(value % base) + converted;
        else                converted = toDigit(value) + converted;

        //if( base == 2 && i+1 == 4 ) converted = " " + converted;
        //if( (i+1)%8 == 0 ) converted = " " + converted;

        value = floor(value / base);
    }

    return converted;
}

//---------------------------------------------------

std::string addQuotes(std::string& str)
{
    return "\"" + str + "\"";
}
std::string remQuotes(std::string& str)
{
    if (str.size() >= 2 && str.front() == '\"' && str.back() == '\"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

std::string removeSubstring(const std::string& original, const std::string& toRemove)
{
    std::string result = original;
    size_t pos = result.find(toRemove);
    if (pos != std::string::npos) 
    {
        result.erase(pos, toRemove.length());
    }
    return result;
}

std::vector<std::string> split(std::string mes, const char s)
{
    std::vector<std::string> list;
    std::stringstream  ss(mes);
    std::string token;
    while (std::getline(ss, token, s))
    {
        if(!token.empty())
            list.push_back(token);
    }
    return list;
}

std::string toLower(const std::string& str)
{
    std::string result = str; // 创建副本以避免修改原始字符串
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string replaceString(std::string subject, const std::string& search, const std::string& replace) 
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) 
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}



std::string_view trim_view(std::string_view sv) {
    const auto start = sv.find_first_not_of(' ');
    if (start == sv.npos) return "";
    const auto end = sv.find_last_not_of(' ');
    return sv.substr(start, end - start + 1);
}

TiXmlDocument fileToDomDoc(const std::string& fileName, const std::string& caller)
{
    TiXmlDocument domDoc;

    // 尝试加载文件
    if (!domDoc.LoadFile(fileName.c_str())) 
    {
        std::cerr << caller << ": " << "Cannot read file:\n" + fileName + "\nError: " + domDoc.ErrorDesc() << std::endl;
        return domDoc;
    }

    // 文件加载成功
    return domDoc;
}

std::string fileToString(const std::string& fileName, const std::string& caller)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        std::cerr << caller << ": Unable to open file " << fileName << std::endl;
        return "";
    }

    // 使用 stringstream 读取文件内容
    std::ostringstream content;
    content << file.rdbuf();
    std::string doc = content.str();

    // 关闭文件
    file.close();

    return doc;
}

//--------------------------------------------------------------

int roundDown(int x, int roundness)
{
    if (x < 0) return (x - roundness + 1) / roundness;
    else        return (x / roundness);
}

int snapToGrid(int x) { return roundDown(x + 2, 4) * 4; }
int snapToCompGrid(int x) { return roundDown(x + 4, 8) * 8; }


