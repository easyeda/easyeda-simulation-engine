/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "numProp.h"

template <class Comp>
class doubleProp :
    public numProp
{
public:
    doubleProp(std::string name, std::string caption, std::string unit, Comp* comp
        , double (Comp::* getter)(), void (Comp::* setter)(double)
        , uint8_t flags = 0, std::string type = "double")
        : numProp(name, caption, unit, type, flags)
    {
        m_comp = comp;
        m_getter = getter;
        m_setter = setter;
    }
    ~doubleProp() { ; }

    virtual void setValStr(std::string val) override
    {
        (m_comp->*m_setter)(getVal(val, m_comp));
    }

    virtual std::string getValStr() override
    {
        return getStr((m_comp->*m_getter)());
    }

    virtual double getValue()
    {
        return (m_comp->*m_getter)();
    }

private:
    Comp* m_comp;
    double (Comp::* m_getter)();
    void   (Comp::* m_setter)(double);

};

