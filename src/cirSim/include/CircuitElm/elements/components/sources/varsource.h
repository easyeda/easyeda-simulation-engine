/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
//#include "voltwidget.h"
#include "e-element.h"

//变量源？
class VarSource: public Component, public eElement
{
    public:
        VarSource( std::string type, std::string id );
        ~VarSource();

        double getVal() { return m_outValue; }
        void setVal( double val );

        double maxValue() { return m_maxValue; }
        void setMaxValue( double v );

        double minValue() { return m_minValue; }
        void setMinValue( double v );

        bool running();
        void setRunning( bool r );

        virtual void initialize() override;

    protected:
        
        double m_maxValue;
        double m_minValue;
        double m_outValue;

        std::string m_unit;
};



