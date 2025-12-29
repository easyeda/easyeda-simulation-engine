/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "pch.h"
#include "logiccomponent.h"

//BCD（Binary-Coded Decimal，二进制编码十进制）的基本功能实现
class BcdBase : public LogicComponent
{
    public:
        BcdBase( std::string type, std::string id );
        ~BcdBase();

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual std::string getElementStatusData() override;
        std::string customChars() { return m_customChars; }
        void setCustomChars( std::string chars );

    protected:
        uint8_t m_segments[17];

        uint8_t m_digit;
        std::string m_customChars;
};

