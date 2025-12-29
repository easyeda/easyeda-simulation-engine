/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "logicfamily.h"
#include "circuit.h"
#include "utils.h"
#include <string_view>

// #include "proputils.h"
using propStr_t = struct {
    std::string_view name;
    std::string_view value;
};
std::unordered_map<std::string, logicFamily_t> LogicFamily::m_families;
std::vector<propStr_t> parseProps(std::string_view line);
propStr_t parseProp(std::string_view token);

LogicFamily::LogicFamily()
{
    m_blocked = false;
    m_enableSupply = true;

    m_supplyV  = 5.0; // Power Supply
    m_inpHighVp = 0.5;
    m_inpHighV  = 2.5;
    m_inpLowVp  = 0.5;
    m_inpLowV   = 2.5;
    m_outHighVp = 1;
    m_outHighV  = 5;
    m_outLowVp  = 0;
    m_outLowV   = 0;

    m_inpImp = 1e9;
    m_outImp = 40;

    m_delayMult = 1;
    m_delayBase = 10*10; // 10 ns
    m_timeLH    = 3000;
    m_timeHL    = 4000;

    m_family = "Custom";
    if( m_families.empty() ) getFamilies();
}
LogicFamily::~LogicFamily(){}

void LogicFamily::setInpHighV( double v )
{
    if( m_blocked ) return;
    m_inpHighVp = v/m_supplyV;
}

void LogicFamily::setInpLowV( double v )
{
    if( m_blocked ) return;
    m_inpLowVp = v/m_supplyV;
}

void LogicFamily::setOutHighV( double v )
{
    if( m_blocked ) return;
    m_outHighVp = v/m_supplyV;
}

void LogicFamily::setOutLowV( double v )
{
    if( m_blocked ) return;
    m_outLowVp = v/m_supplyV;
}

void LogicFamily::setPropDelay( double pd )
{
    if( pd < 0 ) pd = 0;
    if( pd > 1e6   ) pd = 1e6;
    m_delayBase = pd*1e12;
}

void LogicFamily::setRiseTime( double time )
{
    if( time < 1e-12 ) time = 1e-12;
    if( time > 1e6   ) time = 1e6;
    m_timeLH = time*1e12;
}

void LogicFamily::setFallTime( double time )
{
    if( time < 1e-12 ) time = 1e-12;
    if( time > 1e6   ) time = 1e6;
    m_timeHL = time*1e12;
}

void LogicFamily::setFamily( std::string f )
{
    m_family = f;
    if( f == "Custom") setSupplyV( m_supplyV );
    else               setFamilyData( m_families[f]);
}

void LogicFamily::setFamilyData( logicFamily_t lf )
{
    double supplyV = lf.supply;
    m_enableSupply = supplyV < 0;
    if( !m_enableSupply ) m_supplyV = supplyV;

    setSupplyV( m_supplyV );

    m_inpHighVp = lf.inpLHp;
    m_inpLowVp  = lf.inpHLp;
    m_inpImp    = lf.inpImp;

    m_outHighVp = lf.outHip;
    m_outLowVp  = lf.outLop;
    m_outImp    = lf.outImp;

    updateData();
}

void LogicFamily::updateData()
{
    m_blocked = true;

    setInpHighV( m_supplyV * m_inpHighVp );
    setInpLowV(  m_supplyV * m_inpLowVp );
    setInputImp( m_inpImp );
    setInpPullups( m_inpPullups );

    setOutHighV( m_supplyV * m_outHighVp);
    setOutLowV(  m_supplyV * m_outLowVp );
    setOutImp(   m_outImp );
    setOutPullups( m_outPullups );
    m_blocked = false;
}

void LogicFamily::getFamilies() // Static
{
    m_families["Default"] = { -1, 0.5, 0.5, 1e9, 0, 1, 0, 40, 0 };
    m_families["Custom"]  = { -1, 0.5, 0.5, 1e9, 0, 1, 0, 40, 0 };

    std::string modelsFile = Circuit::self()->getDataFilePath("logic_families.model");
    std::ifstream file(modelsFile);
    if (!file.is_open()) return; 


    std::string content = fileToString(modelsFile, "LogicFamily::getFamilies()");
    
    // 按行分割（模拟 splitRef("\n")）
    std::vector<std::string_view> docLines;
    size_t start = 0;
    while (start < content.size()) {
        size_t end = content.find('\n', start);
        if (end == std::string::npos) end = content.size();
        docLines.emplace_back(content.c_str() + start, end - start);
        start = end + 1;
    }
    for (const auto& line : docLines) {
        std::vector<propStr_t> properties = parseProps(line);
        if (properties.empty()) break;
        
        std::string familyName = std::string(properties[0].name);
        logicFamily_t family;
        for (const auto& property : properties) {
            std::string_view propName = property.name;
            double propValue = std::stod(std::string(property.value));
            if      (propName == "supply") family.supply = propValue;
            else if (propName == "inpLHp") family.inpLHp = propValue;
            else if (propName == "inpHLp") family.inpHLp = propValue;
            else if (propName == "inpImp") family.inpImp = propValue;
            else if (propName == "inpPul") family.inpPul = propValue;
            else if (propName == "outHip") family.outHip = propValue;
            else if (propName == "outLop") family.outLop = propValue;
            else if (propName == "outImp") family.outImp = propValue;
            else if (propName == "outPul") family.outPul = propValue;
        }
        m_families[familyName] = family;
    }
}


std::vector<propStr_t> parseProps(std::string_view line) {
    std::vector<propStr_t> properties;
    size_t start = 0;
    while (start < line.size()) {
        const size_t end = line.find(';', start);
        const auto token = line.substr(start, (end == std::string_view::npos) ? 
            line.size() - start : end - start);
        
        auto property = parseProp(token);
        if (!property.name.empty()) {  // 改为检查 .name 而非 .first
            properties.push_back(property);
        }
        start = (end == std::string_view::npos) ? line.size() : end + 1;
    }
    return properties;
}

propStr_t parseProp(std::string_view token) {
    size_t equal_pos = token.find('=');
    if (equal_pos == std::string_view::npos)
        return { token, "" };
    std::string_view key = token.substr(0, equal_pos);
    std::string_view value = token.substr(equal_pos + 1);
    return { key, value };
}

