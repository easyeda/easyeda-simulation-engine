/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "comProperty.h"

comProperty::comProperty(std::string name, std::string caption, std::string unit, std::string type, uint8_t flags)
{
    m_name = name;
    m_capt = caption;
    m_unit = unit;
    m_type = type;
    m_flags = flags;

    //m_widget = NULL;
}
std::string comProperty::name() { return m_name; }
std::string comProperty::capt() { return m_capt; }
std::string comProperty::type() { return m_type; }
std::string comProperty::unit() { return m_unit; }
uint8_t comProperty::flags() { return m_flags; }

void    comProperty::setValStr(std::string) { ; }
std::string comProperty::getValStr() { return "0.0"; }
double  comProperty::getValue() { return 0; }

std::string comProperty::toString() { return getValStr(); }

//void ComProperty::setWidget( PropVal* w ) { m_widget = w; }