/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once
#include "comProperty.h"

class strBaseProp :
    public comProperty
{
public:
    strBaseProp(std::string name, std::string caption, std::string unit, std::string type, uint8_t flags)
        : comProperty(name, caption, unit, type, flags)
    {}
    ~strBaseProp() { ; }

    virtual double getValue() override;

protected:
    std::string getStr(std::string str);
    std::string setStr(std::string str);

};

