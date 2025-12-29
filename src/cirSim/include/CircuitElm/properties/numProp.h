/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "comProperty.h"

class CompBase;

class numProp :
    public comProperty
{
public:
    numProp(std::string name, std::string caption, std::string unit, std::string type, uint8_t flags)
        : comProperty(name, caption, unit, type, flags)
    {}
    ~numProp() { ; }

protected:
    const std::string getValU(double val, std::string& u);

    double getVal(const std::string& val, CompBase* comp);

    std::string getStr(double val);

};

