/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "logiccomponent.h"

class LibraryItem;
// 数字模拟转换器
class DAC : public LogicComponent
{
    public:
        DAC( std::string type, std::string id );
        ~DAC();

 static Component* construct( std::string type, std::string id );
 static LibraryItem *libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override;

        double maxVolt() { return m_maxVolt; }
        void setMaxVolt( double v ) { m_maxVolt = v; }

        void setNumInps( int pins );
        
    protected:
        double m_maxVolt;
        double m_maxValue;

        int m_val;
};

