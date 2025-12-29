/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once


#include "pch.h"
#include <string_view>

 //定义了一组实用工具函数和宏，用于处理文件、字符串、数值单位转换以及其他通用任务。

class Pin;

extern const std::unordered_map<std::string, double> multipliers;

//将电子单位（如纳、微、毫）转换为数值
#define unitToVal( val, mult ) \
    if     ( mult == " n" ) val *= 1e3; \
    else if( mult == " u")  val *= 1e6; \
    else if( mult == " m" ) val *= 1e9; \
    else if( mult == " ")   val *= 1e12;


#define valToUnit( val, mult, decimals ) \
    mult = " p";\
    if( std::fabs( val ) > 999 ) { \
        val /= 1e3; mult = " n"; \
        if( std::fabs( val ) > 999 ) { \
            val /= 1e3; mult = " u"; \
            if( std::fabs( val ) > 999 ) { \
                val /= 1e3; mult = " m"; \
                if( std::fabs( val ) > 999 ) { \
                    val /= 1e3; mult = " "; \
                    if( std::fabs( val ) > 999 ) { \
                        val /= 1e3; mult = " k"; \
                        if( std::fabs( val ) > 999 ) { \
                            val /= 1e3; mult = " M"; \
                            if( std::fabs( val ) > 999 ) { \
                                val /= 1e3; mult = " G"; \
    }}}}}}} \
    if     ( std::fabs( val ) < 10)   decimals = 3; \
    else if( std::fabs( val ) < 100)  decimals = 2; \
    else if( std::fabs( val ) < 1000) decimals = 1;

//数值单位转换函数
double getMultiplier(std::string& mult);  //根据单位前缀返回相应的乘数。
double getValueOfNum(std::string& val);        //根据字符返回数值
std::string multToValStr(double value, std::string& mult);  //将数值和单位前缀转换为字符串表示

//数值和字符串转换函数
std::string val2hex(int d);  //将整数转换为十六进制字符串。
std::string toDigit(int d);  //将整数转换为对应的字符表示（可能用于数字到字符的转换）。
std::string decToBase(int value, int base, int digits); //将十进制数值转换为给定基数（base）的字符串表示。

//---------------------------------------------------

std::string addQuotes(std::string& string);  //添加字符串的引号。
std::string remQuotes(std::string& string);  //移除字符串的引号。
std::string removeSubstring(const std::string & original, const std::string & toRemove);   //删除子字符串
std::vector<std::string> split(std::string mes, const char s);                             //处理字符串
std::string toLower(const std::string & str);                                              //转小写
std::string replaceString(std::string subject, const std::string & search, const std::string & replace); // 替换字符串中的子字符串函数
std::string_view trim_view(std::string_view sv);
// std::vector<propStr_t> parseXmlProps( std::string line );
// std::vector<propStr_t> parseProps(std::string_view line); 
// propStr_t parseProp(std::string_view token); 



//std::string getBareName(std::string& filepath);
//std::string getFileName(std::string& filepath);
//std::string getFileDir(std::string& filepath);
//std::string getFileExt(std::string& filepath);
//std::string changeExt(std::string& filepath, std::string& ext);
//处理文件路径和名称，获取文件的不同部分或更改文件扩展名。

//std::string getDirDialog(std::string& msg, std::string& oldPath);  //打开一个目录选择对话框。
//
//std::string findFile(std::string& dir, std::string& fileName);  //在给定目录中查找文件。

//---------------------------------------------------

TiXmlDocument fileToDomDoc(const std::string & fileName, const std::string & caller);
std::string   fileToString(const std::string& fileName, const std::string& caller);
//QStringList  fileToStringList(const QString& fileName, const QString& caller);
//QByteArray   fileToByteArray(const QString& fileName, const QString& caller);
//读取文件内容并将其转换为不同的格式，如 QDomDocument、字符串、字符串列表或字节数组。
//---------------------------------------------------

int roundDown(int x, int roundness);
int snapToGrid(int x);
int snapToCompGrid(int x);



