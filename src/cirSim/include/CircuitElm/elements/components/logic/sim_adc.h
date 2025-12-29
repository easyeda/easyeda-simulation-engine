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

//模拟数字转换器
class ADC : public LogicComponent
{
    public:
        ADC( std::string type, std::string id );
        ~ADC();

 static Component*   construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        double maxVolt() { return m_maxVolt; }
        void setMaxVolt( double v ) { m_maxVolt = v; }

        void setNumOutputs( int pins );
        
    protected:
        double m_maxVolt;
        double m_maxValue;
};

