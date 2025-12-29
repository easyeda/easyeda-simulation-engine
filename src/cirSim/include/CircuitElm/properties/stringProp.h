/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once
#include "strBaseProp.h"

template <class Comp>
class stringProp :
    public strBaseProp
{
public:
    stringProp(std::string name, std::string caption, std::string unit, Comp* comp
        , std::string(Comp::* getter)(), void (Comp::* setter)(std::string)
        , uint8_t flags = 0, std::string type = "string")
        : strBaseProp(name, caption, unit, type, flags)
    {
        m_comp = comp;
        m_getter = getter;
        m_setter = setter;
    }
    ~stringProp() { ; }

    virtual void setValStr(std::string val) override
    {
        /// if( m_comp->showProp() == m_name ) m_comp->setValLabelText( val );  // Needed???
        (m_comp->*m_setter)(setStr(val)); // Comp setter can change valLabel
    }

    virtual std::string getValStr() override
    {
        return (m_comp->*m_getter)();
    }

    virtual std::string toString() { return getStr((m_comp->*m_getter)()); }

private:
    Comp* m_comp;
    std::string(Comp::* m_getter)();
    void    (Comp::* m_setter)(std::string);
};

