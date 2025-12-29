/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once


#include "pch.h"
//设置属性的标志。
enum propFlags {
    propHidden = 1,
    propNoCopy = 1 << 1
};

//元件属性类
class comProperty
{
public:
    comProperty(std::string name, std::string caption, std::string unit, std::string type, uint8_t flags);
    virtual ~comProperty() { ; }

    //获取属性的名称、标题、类型、单位和标志。
    std::string name();
    std::string capt();
    std::string type();
    std::string unit();
    uint8_t flags();

    //设置和获取属性的字符串值和数值。
    virtual void    setValStr(std::string);
    virtual std::string getValStr();
    virtual double  getValue();

    // toString，用于将属性转换为字符串表示。
    virtual std::string toString();

    //void setWidget( PropVal* w );

protected:

    std::string m_name;
    std::string m_capt;
    std::string m_type;
    std::string m_unit;
    uint8_t m_flags;
};

