/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "numProp.h"
#include "utils.h"
#include "compbase.h"


const std::string numProp::getValU(double val, std::string& u)
{
    std::ostringstream stream;
    stream << val << " " << u;
    return stream.str();
}

std::string numProp::getStr(double val)
{
    std::string valStr;
    //if( m_widget ) valStr = m_widget->getValWithUnit();
    //else
    {
        double multiplier = getMultiplier(m_unit);
        std::ostringstream stream;
        stream << val / multiplier;
        valStr = stream.str();
        if (!m_unit.empty()) valStr.append(" " + m_unit);
    }
    return valStr;
}

double numProp::getVal(const std::string& val, CompBase* comp)
{
    std::istringstream iss(val);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token)
        tokens.push_back(token);

    double v = 0.0;
    if(!tokens.empty()){
        try{
            size_t pos = 0;
            v = std::stod(tokens.front(),&pos);
            if(pos != tokens.front().length()){
                throw std::invalid_argument("invalid characters in number");
            }
        } catch ( const std:: exception& e){
            std::cerr << "Error converting " << tokens.front()
                      << " to double: " << e.what() << std::endl;
            v = 0.0;
        }
    }

    if (tokens.size() > 1)
    {
        std::string unit = tokens.back();
        if (unit.front() != '_') m_unit = unit;
    }

    return v * getMultiplier(m_unit);
}