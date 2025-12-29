/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#pragma once

#include "comProperty.h"

template <class Comp>
class boolProp :
    public comProperty
{
public:
    boolProp(std::string name, std::string caption, std::string unit, Comp* comp
        , bool (Comp::* getter)(), void (Comp::* setter)(bool)
        , uint8_t flags = 0, std::string type = "bool")
        : comProperty(name, caption, unit, type, flags)
    {
        m_comp = comp;
        m_getter = getter;
        m_setter = setter;
    }
    ~boolProp() { ; }

    virtual void setValStr(std::string val) override
    {
        (m_comp->*m_setter)(val == "true");
    }

    virtual std::string getValStr() override
    {
        return (m_comp->*m_getter)() ? "true" : "false";
    }

private:
    Comp* m_comp;
    bool (Comp::* m_getter)();
    void (Comp::* m_setter)(bool);
};

